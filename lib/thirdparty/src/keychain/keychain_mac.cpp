/*
 * Copyright (c) 2013 GitHub Inc.
 * Copyright (c) 2019 Hannes Rantzsch
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <type_traits>
#include <vector>

#include <Security/Security.h>

#include "keychain.h"

namespace {

/*! \brief Converts a CFString to a std::string
 *
 * This either uses CFStringGetCStringPtr or (if that fails) CFStringGetCString.
 */
std::string CFStringToStdString(const CFStringRef cfstring) {
    const char *ccstr = CFStringGetCStringPtr(cfstring, kCFStringEncodingUTF8);

    if (ccstr != nullptr) {
        return std::string(ccstr);
    }

    auto utf16Pairs = CFStringGetLength(cfstring);
    auto maxUtf8Bytes =
        CFStringGetMaximumSizeForEncoding(utf16Pairs, kCFStringEncodingUTF8);

    std::vector<char> cstr(maxUtf8Bytes, '\0');
    auto result = CFStringGetCString(
        cfstring, cstr.data(), cstr.size(), kCFStringEncodingUTF8);

    return result ? std::string(cstr.data()) : std::string();
}

//! \brief Extracts a human readable string from a status code
std::string errorStatusToString(OSStatus status) {
    const auto errorMessage = SecCopyErrorMessageString(status, NULL);
    std::string errorString;

    if (errorMessage) {
        errorString = CFStringToStdString(errorMessage);
        CFRelease(errorMessage);
    }

    return errorString;
}

std::string makeServiceName(const std::string &package,
                            const std::string &service) {
    return package + "." + service;
}

/*! \brief Update error information
 *
 * If status indicates an error condition, set message, code and error type.
 * Otherwise, set err to success.
 */
void updateError(keychain::Error &err, OSStatus status) {
    if (status == errSecSuccess) {
        err = keychain::Error{};
        return;
    }

    err.message = errorStatusToString(status);
    err.code = status;

    switch (status) {
    case errSecItemNotFound:
        err.type = keychain::ErrorType::NotFound;
        break;

    // potential errors in case the user needs to unlock the keychain first
    case errSecUserCanceled:        // user pressed the Cancel button
    case errSecAuthFailed:          // too many failed password attempts
    case errSecInteractionRequired: // user interaction required but not allowed
        err.type = keychain::ErrorType::AccessDenied;
        break;

    default:
        err.type = keychain::ErrorType::GenericError;
    }
}

void setGenericError(keychain::Error &err, const std::string &errorMessage) {
    err = keychain::Error{};
    err.message = errorMessage;
    err.type = keychain::ErrorType::GenericError;
    err.code = -1;
}

/*! \brief Helper to manage the lifetime of CF-Objects
 *
 * This helper will CFRelease the managed CF-Object when it goes out of scope.
 * It assumes ownership of the managed object, so users should own the object in
 * terms of the Core Foundation "Create Rule" when passing it to the
 * ScopedCFRef. Consequently, the object should also not be released by anyone
 * else, at least not without calling CFRetain first.
 * */
template <typename T,
          typename = typename std::enable_if<std::is_pointer<T>::value>::type>
class ScopedCFRef {
  public:
    explicit ScopedCFRef(T ref) : _ref(ref) {}
    ~ScopedCFRef() { _release(); }

    ScopedCFRef(ScopedCFRef &&other) noexcept : _ref(other._ref) {
        other._ref = nullptr;
    }
    ScopedCFRef &operator=(ScopedCFRef &&other) {
        if (this != &other) {
            _release();
            _ref = other._ref;
            other._ref = nullptr;
        }
        return *this;
    }

    ScopedCFRef(const ScopedCFRef &) = delete;
    ScopedCFRef &operator=(const ScopedCFRef &) = delete;

    const T get() const { return _ref; }
    operator bool() const { return _ref != nullptr; }

  private:
    void _release() {
        if (_ref != nullptr) {
            CFRelease(_ref);
            _ref = nullptr;
        }
    }

    T _ref;
};

ScopedCFRef<CFStringRef> createCFStringWithCString(const std::string &str,
                                                   keychain::Error &err) {
    auto result = ScopedCFRef<CFStringRef>(CFStringCreateWithCString(
        kCFAllocatorDefault, str.c_str(), kCFStringEncodingUTF8));
    if (!result)
        setGenericError(err, "Failed to create CFString");
    return result;
}

ScopedCFRef<CFMutableDictionaryRef>
createCFMutableDictionary(keychain::Error &err) {
    auto result = ScopedCFRef<CFMutableDictionaryRef>(
        CFDictionaryCreateMutable(kCFAllocatorDefault,
                                  0,
                                  &kCFTypeDictionaryKeyCallBacks,
                                  &kCFTypeDictionaryValueCallBacks));
    if (!result)
        setGenericError(err, "Failed to create CFMutableDictionary");
    return result;
}

ScopedCFRef<CFDataRef> createCFData(const std::string &data,
                                    keychain::Error &err) {
    auto result = ScopedCFRef<CFDataRef>(
        CFDataCreate(kCFAllocatorDefault,
                     reinterpret_cast<const UInt8 *>(data.c_str()),
                     data.length()));
    if (!result)
        setGenericError(err, "Failed to create CFData");
    return result;
}

ScopedCFRef<CFMutableDictionaryRef> createQuery(const std::string &serviceName,
                                                const std::string &user,
                                                keychain::Error &err) {
    const auto cfServiceName = createCFStringWithCString(serviceName, err);
    const auto cfUser = createCFStringWithCString(user, err);
    auto query = createCFMutableDictionary(err);

    if (err.type != keychain::ErrorType::NoError)
        return query;

    CFDictionaryAddValue(query.get(), kSecClass, kSecClassGenericPassword);
    CFDictionaryAddValue(query.get(), kSecAttrAccount, cfUser.get());
    CFDictionaryAddValue(query.get(), kSecAttrService, cfServiceName.get());

    return query;
}

} // namespace

namespace keychain {

void setPassword(const std::string &package, const std::string &service,
                 const std::string &user, const std::string &password,
                 Error &err) {
    err = Error{};
    const auto serviceName = makeServiceName(package, service);
    const auto cfPassword = createCFData(password, err);
    auto query = createQuery(serviceName, user, err);

    if (err.type != keychain::ErrorType::NoError)
        return;

    CFDictionaryAddValue(query.get(), kSecValueData, cfPassword.get());
    OSStatus status = SecItemAdd(query.get(), NULL);

    if (status == errSecDuplicateItem) {
        // password exists -- override
        auto attributesToUpdate = createCFMutableDictionary(err);
        if (err.type != keychain::ErrorType::NoError)
            return;

        CFDictionaryAddValue(
            attributesToUpdate.get(), kSecValueData, cfPassword.get());
        status = SecItemUpdate(query.get(), attributesToUpdate.get());
    }

    updateError(err, status);
}

std::string getPassword(const std::string &package, const std::string &service,
                        const std::string &user, Error &err) {
    err = Error{};
    const auto serviceName = makeServiceName(package, service);
    auto query = createQuery(serviceName, user, err);

    if (err.type != keychain::ErrorType::NoError)
        return "";

    CFDictionaryAddValue(query.get(), kSecReturnData, kCFBooleanTrue);

    CFTypeRef result = nullptr;
    updateError(err, SecItemCopyMatching(query.get(), &result));
    const auto cfPassword = ScopedCFRef<CFDataRef>((CFDataRef)result);

    if (!cfPassword || err.type != keychain::ErrorType::NoError)
        return "";

    return std::string(
        reinterpret_cast<const char *>(CFDataGetBytePtr(cfPassword.get())),
        CFDataGetLength(cfPassword.get()));
}

void deletePassword(const std::string &package, const std::string &service,
                    const std::string &user, Error &err) {
    err = Error{};
    const auto serviceName = makeServiceName(package, service);
    const auto query = createQuery(serviceName, user, err);

    if (err.type != keychain::ErrorType::NoError)
        return;

    updateError(err, SecItemDelete(query.get()));
}

} // namespace keychain
