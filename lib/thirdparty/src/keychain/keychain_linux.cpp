/*
 * Copyright (c) 2013 GitHub Inc.
 * Copyright (c) 2015-2019 Vaclav Slavik
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

#include "keychain.h"

#include <libsecret/secret.h>

namespace {

const char *ServiceFieldName = "service";
const char *AccountFieldName = "username";

// disable warnings about missing initializers in SecretSchema
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif

SecretSchema makeSchema(const std::string &package) {
    return SecretSchema{package.c_str(),
                        SECRET_SCHEMA_NONE,
                        {
                            {ServiceFieldName, SECRET_SCHEMA_ATTRIBUTE_STRING},
                            {AccountFieldName, SECRET_SCHEMA_ATTRIBUTE_STRING},
                            {NULL, SecretSchemaAttributeType(0)},
                        }};
}

std::string makeLabel(const std::string &service, const std::string &user) {
    std::string label = service;

    if (!user.empty()) {
        label += " (" + user + ")";
    }

    return label;
}

void updateError(keychain::Error &err, GError *error) {
    if (error == NULL) {
        err = keychain::Error{};
        return;
    }

    err.type = keychain::ErrorType::GenericError;
    err.message = error->message;
    err.code = error->code;
    g_error_free(error);
}

void setErrorNotFound(keychain::Error &err) {
    err.type = keychain::ErrorType::NotFound;
    err.message = "Password not found.";
    err.code = -1; // generic non-zero
}

} // namespace

namespace keychain {

void setPassword(const std::string &package, const std::string &service,
                 const std::string &user, const std::string &password,
                 Error &err) {
    err = Error{};
    const auto schema = makeSchema(package);
    const auto label = makeLabel(service, user);
    GError *error = NULL;

    secret_password_store_sync(&schema,
                               SECRET_COLLECTION_DEFAULT,
                               label.c_str(),
                               password.c_str(),
                               NULL, // not cancellable
                               &error,
                               ServiceFieldName,
                               service.c_str(),
                               AccountFieldName,
                               user.c_str(),
                               NULL);

    if (error != NULL) {
        updateError(err, error);
    }
}

std::string getPassword(const std::string &package, const std::string &service,
                        const std::string &user, Error &err) {
    err = Error{};
    const auto schema = makeSchema(package);
    GError *error = NULL;

    gchar *raw_passwords = secret_password_lookup_sync(&schema,
                                                       NULL, // not cancellable
                                                       &error,
                                                       ServiceFieldName,
                                                       service.c_str(),
                                                       AccountFieldName,
                                                       user.c_str(),
                                                       NULL);

    std::string password;

    if (error != NULL) {
        updateError(err, error);
    } else if (raw_passwords == NULL) {
        // libsecret reports no error if the password was not found
        setErrorNotFound(err);
    } else {
        password = raw_passwords;
        secret_password_free(raw_passwords);
    }

    return password;
}

void deletePassword(const std::string &package, const std::string &service,
                    const std::string &user, Error &err) {
    err = Error{};
    const auto schema = makeSchema(package);
    GError *error = NULL;

    bool deleted = secret_password_clear_sync(&schema,
                                              NULL, // not cancellable
                                              &error,
                                              ServiceFieldName,
                                              service.c_str(),
                                              AccountFieldName,
                                              user.c_str(),
                                              NULL);

    if (error != NULL) {
        updateError(err, error);
    } else if (!deleted) {
        // libsecret reports no error if the password did not exist
        setErrorNotFound(err);
    }
}

} // namespace keychain
