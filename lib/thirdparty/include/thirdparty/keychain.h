/*
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

#ifndef XPLATFORM_KEYCHAIN_WRAPPER_H_
#define XPLATFORM_KEYCHAIN_WRAPPER_H_

#include <string>

/*! \brief A thin wrapper to provide cross-platform access to the operating
 *         system's credentials storage.
 *
 * keychain provides the functions getPassword, setPassword, and deletePassword.
 *
 * All of these functions require three input parameters to identify the
 * credential that should be retrieved or manipulated: `package`, `service`, and
 * `user`. These identifiers will be mangled differently on each OS to
 * correspond to the OS API.
 * While none of the supported OSes has specific requirements to the format
 * identifiers, the reverse domain name format is recommended for the `package`
 * parameter in order to correspond with conventions.
 *
 * In addition, each function expects an instance of `keychain::Error` as an
 * output parameter to indicate success or failure. Note that previous states of
 * the Error are ignored and potentially overwritten.
 *
 * Also note that all three functions are blocking (potentially indefinitely)
 * for example if the OS prompts the user to unlock their credentials storage.
 */
namespace keychain {

struct Error;

/*! \brief Retrieve a password
 *
 * \param package, service, user Used to identify the password to get
 * \param err Output parameter communicating success or error details
 *
 * \return The password, if the function was successful
 */
std::string getPassword(const std::string &package, const std::string &service,
                        const std::string &user, Error &err);

/*! \brief Insert or update a password
 *
 * Existing passwords will be overwritten.
 *
 * \param package, service, user Used to identify the password to set
 * \param password The new password
 * \param err Output parameter communicating success or error details
 */
void setPassword(const std::string &package, const std::string &service,
                 const std::string &user, const std::string &password,
                 Error &err);

/*! \brief Insert or update a password
 *
 * Trying to delete a password that does not exist will result in a NotFound
 * error.
 *
 * \param package, service, user Used to identify the password to delete
 * \param err Output parameter communicating success or error details
 */
void deletePassword(const std::string &package, const std::string &service,
                    const std::string &user, Error &err);

enum class ErrorType {
    // update CATCH_REGISTER_ENUM in tests.cpp when changing this
    NoError = 0,
    GenericError,
    NotFound,
    // OS-specific errors
    PasswordTooLong = 10, // Windows only
    AccessDenied,         // macOS only
};

/*! \brief A struct to collect error information
 *
 * An instance of this struct is used as an output parameter to indicate success
 * or failure.
 */
struct Error {
    Error() : type(ErrorType::NoError) {}

    /*! \brief The type or reason of the error
     *
     * Note that some types of errors can only occur on certain platforms. In
     * cases where a platform-specific error occurs on one platform, both
     * NoError or some other (more generic) error might occur on others.
     */
    ErrorType type;

    /*! \brief A human-readable explanatory error message
     *
     * In most cases this message is obtained from the operating system.
     */
    std::string message;

    /*! \brief The "native" error code set by the operating system
     *
     * Even for the same type of error this value will differ across platforms.
     */
    int code;

    //! \brief Checks if the error type is not NoError
    operator bool() const { return ErrorType::NoError != type; }
};

} // namespace keychain

#endif
