/*
 * Copyright (c) 2013 GitHub Inc.
 * Copyright (c) 2019 Hannes Rantzsch, René Meusel
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

// clang-format off
// make sure windows.h is included before wincred.h
#include "keychain.h"

#include <memory>

#define UNICODE

#include <windows.h>
#include <wincred.h>
#define DWORD_MAX 0xffffffffUL
// clang-format on

namespace {

static const DWORD kCredType = CRED_TYPE_GENERIC;

struct LpwstrDeleter {
    void operator()(WCHAR *p) const { delete[] p; }
};

//! Wrapper around a WCHAR pointer a.k.a. LPWStr to take care of memory handling
using ScopedLpwstr = std::unique_ptr<WCHAR, LpwstrDeleter>;

/*! \brief Converts a UTF-8 std::string to wide char
 *
 * Uses MultiByteToWideChar to convert the input string and wraps the result in
 * a ScopedLpwstr. Returns nullptr on failure.
 */
ScopedLpwstr utf8ToWideChar(const std::string &utf8) {
    int requiredBufSize = MultiByteToWideChar(
        CP_UTF8,
        0, // flags must be 0 for UTF-8
        utf8.c_str(),
        -1,      // rely on null-terminated input string
        nullptr, // no out-buffer needed
        0);      // return required buffer size; don't write to out-buffer

    // 0 means MultiByteToWideChar did not succeed. Note that even an empty
    // string yields 1 on success due to the terminating null character needed
    // in the out-buffer.
    if (requiredBufSize == 0) {
        return nullptr;
    }

    ScopedLpwstr lwstr(new WCHAR[requiredBufSize]);
    int bytesWritten = MultiByteToWideChar(
        CP_UTF8, 0, utf8.c_str(), -1, lwstr.get(), requiredBufSize);

    if (bytesWritten == 0) {
        return nullptr;
    }

    return lwstr;
}

/*! \brief Converts a wide char pointer to a std::string
 *
 * Note that this function provides no reliable indication of errors and simply
 * returns an empty string in case it fails.
 */
std::string wideCharToAnsi(LPWSTR wChar) {
    std::string result;
    if (wChar == nullptr) {
        return result;
    }

    int requiredBufSize = WideCharToMultiByte(
        CP_ACP,
        0, // flags
        wChar,
        -1,       // rely on null-terminated input string
        nullptr,  // no out-buffer needed
        0,        // return required buffer size; don't write to out-buffer
        nullptr,  // use system default for non representable characters
        nullptr); // unused output parameter

    // 0 indicates error; see comment in utf8ToWideChar.
    if (requiredBufSize == 0) {
        return result;
    }

    std::unique_ptr<char[]> buffer(new char[requiredBufSize]);
    int bytesWritten = WideCharToMultiByte(
        CP_ACP, 0, wChar, -1, buffer.get(), requiredBufSize, nullptr, nullptr);

    if (bytesWritten != 0) {
        result = std::string(buffer.get());
    }

    return result;
}

/*! /brief Get an explanatory message for an error code obtained via
 * ::GetLastError()
 */
std::string getErrorMessage(DWORD errorCode) {
    std::string errMsg;
    LPWSTR errBuffer = nullptr;
    auto written = ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        nullptr, // ignored for the flags we use
        errorCode,
        0, // figure out LANGID automatically
        reinterpret_cast<LPWSTR>(&errBuffer),
        0,        // figure out out-buffer size automatically
        nullptr); // no additional arguments

    if (written > 0 && errBuffer != nullptr) {
        errMsg = wideCharToAnsi(errBuffer);
        LocalFree(errBuffer);
    }
    return errMsg;
}

void updateError(keychain::Error &err) {
    const auto code = ::GetLastError();
    if (code == ERROR_SUCCESS) {
        err = keychain::Error{};
        return;
    }

    err.message = getErrorMessage(code);
    err.code = code;
    err.type = err.code == ERROR_NOT_FOUND ? keychain::ErrorType::NotFound
                                           : keychain::ErrorType::GenericError;
}

/*! /brief Create the target name used to lookup and store credentials
 *
 * The result is wrapped in a ScopedLpwstr.
 */
ScopedLpwstr makeTargetName(const std::string &package,
                            const std::string &service, const std::string &user,
                            keychain::Error &err) {
    auto result = utf8ToWideChar(package + "." + service + '/' + user);
    if (!result) {
        updateError(err);

        // make really sure that we set an error code if we will return nullptr
        if (!err) {
            err.type = keychain::ErrorType::GenericError;
            err.message = "Failed to create credential target name.";
            err.code = -1; // generic non-zero
        }
    }

    return result;
}

} // namespace

namespace keychain {

void setPassword(const std::string &package, const std::string &service,
                 const std::string &user, const std::string &password,
                 Error &err) {
    err = Error{};
    auto target_name = makeTargetName(package, service, user, err);
    if (err) {
        return;
    }

    ScopedLpwstr user_name(utf8ToWideChar(user));
    if (!user_name) {
        updateError(err);
        return;
    }

    if (password.size() > CRED_MAX_CREDENTIAL_BLOB_SIZE ||
        password.size() > DWORD_MAX) {
        err.type = ErrorType::PasswordTooLong;
        err.message = "Password too long.";
        err.code = -1; // generic non-zero
        return;
    }

    CREDENTIAL cred = {};
    cred.Type = kCredType;
    cred.TargetName = target_name.get();
    cred.UserName = user_name.get();
    cred.CredentialBlobSize = static_cast<DWORD>(password.size());
    cred.CredentialBlob = (LPBYTE)(password.data());
    cred.Persist = CRED_PERSIST_ENTERPRISE;

    if (::CredWrite(&cred, 0) == FALSE) {
        updateError(err);
    }
}

std::string getPassword(const std::string &package, const std::string &service,
                        const std::string &user, Error &err) {
    err = Error{};
    std::string password;

    auto target_name = makeTargetName(package, service, user, err);
    if (err) {
        return password;
    }

    CREDENTIAL *cred;
    bool result = ::CredRead(target_name.get(), kCredType, 0, &cred);

    if (result == TRUE) {
        password = std::string(reinterpret_cast<char *>(cred->CredentialBlob),
                               cred->CredentialBlobSize);
        ::CredFree(cred);
    } else {
        updateError(err);
    }

    return password;
}

void deletePassword(const std::string &package, const std::string &service,
                    const std::string &user, Error &err) {
    err = Error{};
    auto target_name = makeTargetName(package, service, user, err);
    if (err) {
        return;
    }

    if (::CredDelete(target_name.get(), kCredType, 0) == FALSE) {
        updateError(err);
    }
}

} // namespace keychain
