// SPDX-FileCopyrightText: Copyright 2018-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
// SPDX-License-Identifier: Apache-2.0

/* This file contains reference definitions for implementation of the
 * PSA Certified Crypto API v1.1 PAKE Extension beta.1
 *
 * These definitions must be embedded in, or included by, psa/crypto.h
 */


/**
 * @brief Whether the specified algorithm is a password-authenticated key
 *        exchange.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a password-authenticated key exchange (PAKE) algorithm, 0
 *         otherwise.
 */
#define PSA_ALG_IS_PAKE(alg) /* specification-defined value */

/**
 * @brief The Password-authenticated key exchange by juggling (J-PAKE)
 *        algorithm.
 */
#define PSA_ALG_JPAKE ((psa_algorithm_t)0x0a000100)

/**
 * @brief Encoding of the type of the PAKE's primitive.
 */
typedef uint8_t psa_pake_primitive_type_t;

/**
 * @brief The PAKE primitive type indicating the use of elliptic curves.
 */
#define PSA_PAKE_PRIMITIVE_TYPE_ECC ((psa_pake_primitive_type_t)0x01)

/**
 * @brief The PAKE primitive type indicating the use of Diffie-Hellman groups.
 */
#define PSA_PAKE_PRIMITIVE_TYPE_DH ((psa_pake_primitive_type_t)0x02)

/**
 * @brief Encoding of the family of the primitive associated with the PAKE.
 */
typedef uint8_t psa_pake_family_t;

/**
 * @brief Encoding of the primitive associated with the PAKE.
 */
typedef uint32_t psa_pake_primitive_t;

/**
 * @brief Construct a PAKE primitive from type, family and bit-size.
 *
 * @param pake_type   The type of the primitive: a value of type
 *                    psa_pake_primitive_type_t.
 * @param pake_family The family of the primitive.
 * @param pake_bits   The bit-size of the primitive: a value of type size_t.
 *
 * @return The constructed primitive value.
 */
#define PSA_PAKE_PRIMITIVE(pake_type, pake_family, pake_bits) \
    /* specification-defined value */

/**
 * @brief The type of an object describing a PAKE cipher suite.
 */
typedef /* implementation-defined type */ psa_pake_cipher_suite_t;

/**
 * @brief This macro returns a suitable initializer for a PAKE cipher suite
 *        object of type psa_pake_cipher_suite_t.
 */
#define PSA_PAKE_CIPHER_SUITE_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a PAKE cipher suite object.
 */
psa_pake_cipher_suite_t psa_pake_cipher_suite_init(void);

/**
 * @brief Retrieve the PAKE algorithm from a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to query.
 *
 * @return The PAKE algorithm stored in the cipher suite object.
 */
psa_algorithm_t psa_pake_cs_get_algorithm(const psa_pake_cipher_suite_t* cipher_suite);

/**
 * @brief Declare the PAKE algorithm for the cipher suite.
 *
 * @param cipher_suite The cipher suite object to write to.
 * @param alg          The PAKE algorithm to write: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_PAKE(alg) is true.
 */
void psa_pake_cs_set_algorithm(psa_pake_cipher_suite_t* cipher_suite,
                               psa_algorithm_t alg);

/**
 * @brief Retrieve the primitive from a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to query.
 *
 * @return The primitive stored in the cipher suite object.
 */
psa_pake_primitive_t psa_pake_cs_get_primitive(const psa_pake_cipher_suite_t* cipher_suite);

/**
 * @brief Declare the primitive for a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to write to.
 * @param primitive    The PAKE primitive to write: a value of type
 *                     psa_pake_primitive_t.
 */
void psa_pake_cs_set_primitive(psa_pake_cipher_suite_t* cipher_suite,
                               psa_pake_primitive_t primitive);

/**
 * @brief Retrieve the hash algorithm from a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to query.
 *
 * @return The hash algorithm stored in the cipher suite object.
 */
psa_pake_primitive_t psa_pake_cs_get_hash(const psa_pake_cipher_suite_t* cipher_suite);

/**
 * @brief Declare the hash algorithm for a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to write to.
 * @param hash_alg     The hash algorithm to write: a value of type
 *                     psa_algorithm_t such that PSA_ALG_IS_HASH(hash_alg) is
 *                     true.
 */
void psa_pake_cs_set_hash(psa_pake_cipher_suite_t* cipher_suite,
                          psa_algorithm_t hash_alg);

/**
 * @brief Encoding of the application role in a PAKE algorithm.
 */
typedef uint8_t psa_pake_role_t;

/**
 * @brief A value to indicate no role in a PAKE algorithm.
 */
#define PSA_PAKE_ROLE_NONE ((psa_pake_role_t)0x00)

/**
 * @brief The first peer in a balanced PAKE.
 */
#define PSA_PAKE_ROLE_FIRST ((psa_pake_role_t)0x01)

/**
 * @brief The second peer in a balanced PAKE.
 */
#define PSA_PAKE_ROLE_SECOND ((psa_pake_role_t)0x02)

/**
 * @brief The client in an augmented PAKE.
 */
#define PSA_PAKE_ROLE_CLIENT ((psa_pake_role_t)0x11)

/**
 * @brief The server in an augmented PAKE.
 */
#define PSA_PAKE_ROLE_SERVER ((psa_pake_role_t)0x12)

/**
 * @brief Encoding of input and output steps for a PAKE algorithm.
 */
typedef uint8_t psa_pake_step_t;

/**
 * @brief The key share being sent to or received from the peer.
 */
#define PSA_PAKE_STEP_KEY_SHARE ((psa_pake_step_t)0x01)

/**
 * @brief A Schnorr NIZKP public key.
 */
#define PSA_PAKE_STEP_ZK_PUBLIC ((psa_pake_step_t)0x02)

/**
 * @brief A Schnorr NIZKP proof.
 */
#define PSA_PAKE_STEP_ZK_PROOF ((psa_pake_step_t)0x03)

/**
 * @brief The type of the state object for PAKE operations.
 */
typedef /* implementation-defined type */ psa_pake_operation_t;

/**
 * @brief This macro returns a suitable initializer for a PAKE operation object
 *        of type psa_pake_operation_t.
 */
#define PSA_PAKE_OPERATION_INIT /* implementation-defined value */

/**
 * @brief Return an initial value for a PAKE operation object.
 */
psa_pake_operation_t psa_pake_operation_init(void);

/**
 * @brief Set the session information for a password-authenticated key exchange.
 *
 * @param operation    The operation object to set up.
 * @param cipher_suite The cipher suite to use.
 */
psa_status_t psa_pake_setup(psa_pake_operation_t *operation,
                            const psa_pake_cipher_suite_t *cipher_suite);

/**
 * @brief Set the password for a password-authenticated key exchange using a
 *        key.
 *
 * @param operation Active PAKE operation.
 * @param password  Identifier of the key holding the password or a value
 *                  derived from the password.
 */
psa_status_t psa_pake_set_password_key(psa_pake_operation_t *operation,
                                       psa_key_id_t password);

/**
 * @brief Set the user ID for a password-authenticated key exchange.
 *
 * @param operation   Active PAKE operation.
 * @param user_id     The user ID to authenticate with.
 * @param user_id_len Size of the user_id buffer in bytes.
 */
psa_status_t psa_pake_set_user(psa_pake_operation_t *operation,
                               const uint8_t *user_id,
                               size_t user_id_len);

/**
 * @brief Set the peer ID for a password-authenticated key exchange.
 *
 * @param operation   Active PAKE operation.
 * @param peer_id     The peer's ID to authenticate.
 * @param peer_id_len Size of the peer_id buffer in bytes.
 */
psa_status_t psa_pake_set_peer(psa_pake_operation_t *operation,
                               const uint8_t *peer_id,
                               size_t peer_id_len);

/**
 * @brief Set the application role for a password-authenticated key exchange.
 *
 * @param operation Active PAKE operation.
 * @param role      A value of type psa_pake_role_t indicating the application
 *                  role in the PAKE algorithm.
 */
psa_status_t psa_pake_set_role(psa_pake_operation_t *operation,
                               psa_pake_role_t role);

/**
 * @brief Get output for a step of a password-authenticated key exchange.
 *
 * @param operation     Active PAKE operation.
 * @param step          The step of the algorithm for which the output is
 *                      requested.
 * @param output        Buffer where the output is to be written.
 * @param output_size   Size of the output buffer in bytes.
 * @param output_length On success, the number of bytes of the returned output.
 */
psa_status_t psa_pake_output(psa_pake_operation_t *operation,
                             psa_pake_step_t step,
                             uint8_t *output,
                             size_t output_size,
                             size_t *output_length);

/**
 * @brief Provide input for a step of a password-authenticated key exchange.
 *
 * @param operation    Active PAKE operation.
 * @param step         The step for which the input is provided.
 * @param input        Buffer containing the input.
 * @param input_length Size of the input buffer in bytes.
 */
psa_status_t psa_pake_input(psa_pake_operation_t *operation,
                            psa_pake_step_t step,
                            const uint8_t *input,
                            size_t input_length);

/**
 * @brief Pass the implicitly confirmed shared secret from a PAKE into a key
 *        derivation operation.
 *
 * @param operation Active PAKE operation.
 * @param output    A key derivation operation that is ready for an input step
 *                  of type PSA_KEY_DERIVATION_INPUT_SECRET.
 */
psa_status_t psa_pake_get_implicit_key(psa_pake_operation_t *operation,
                                       psa_key_derivation_operation_t *output);

/**
 * @brief Abort a PAKE operation.
 *
 * @param operation Initialized PAKE operation.
 */
psa_status_t psa_pake_abort(psa_pake_operation_t * operation);

/**
 * @brief Sufficient output buffer size for psa_pake_output(), in bytes.
 *
 * @param alg         A PAKE algorithm: a value of type psa_algorithm_t such
 *                    that PSA_ALG_IS_PAKE(alg) is true.
 * @param primitive   A primitive of type psa_pake_primitive_t that is
 *                    compatible with algorithm alg.
 * @param output_step A value of type psa_pake_step_t that is valid for the
 *                    algorithm alg.
 *
 * @return A sufficient output buffer size for the specified PAKE algorithm,
 *         primitive, and output step.
 */
#define PSA_PAKE_OUTPUT_SIZE(alg, primitive, output_step) \
    /* implementation-defined value */

/**
 * @brief Sufficient output buffer size for psa_pake_output() for any of the
 *        supported PAKE algorithms, primitives and output steps.
 */
#define PSA_PAKE_OUTPUT_MAX_SIZE /* implementation-defined value */

/**
 * @brief Sufficient buffer size for inputs to psa_pake_input().
 *
 * @param alg        A PAKE algorithm: a value of type psa_algorithm_t such that
 *                   PSA_ALG_IS_PAKE(alg) is true.
 * @param primitive  A primitive of type psa_pake_primitive_t that is compatible
 *                   with algorithm alg.
 * @param input_step A value of type psa_pake_step_t that is valid for the
 *                   algorithm alg.
 *
 * @return A sufficient buffer size for the specified PAKE algorithm, primitive,
 *         and input step.
 */
#define PSA_PAKE_INPUT_SIZE(alg, primitive, input_step) \
    /* implementation-defined value */

/**
 * @brief Sufficient buffer size for inputs to psa_pake_input() for any of the
 *        supported PAKE algorithms, primitives and input steps.
 */
#define PSA_PAKE_INPUT_MAX_SIZE /* implementation-defined value */
