// SPDX-FileCopyrightText: Copyright 2018-2024 Arm Limited and/or its affiliates <open-source-office@arm.com>
// SPDX-License-Identifier: Apache-2.0

/* This file contains reference definitions for implementation of the
 * PSA Certified Crypto API v1.2 PAKE Extension
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
 * @brief Encoding of the primitive associated with the PAKE.
 */
typedef uint32_t psa_pake_primitive_t;

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
 * @brief Extract the PAKE primitive type from a PAKE primitive.
 *
 * @param pake_primitive A PAKE primitive: a value of type psa_pake_primitive_t.
 *
 * @return The PAKE primitive type, if pake_primitive is a supported PAKE
 *         primitive.
 */
#define PSA_PAKE_PRIMITIVE_GET_TYPE(pake_primitive) \
    /* specification-defined value */

/**
 * @brief Extract the family from a PAKE primitive.
 *
 * @param pake_primitive A PAKE primitive: a value of type psa_pake_primitive_t.
 *
 * @return The PAKE primitive family, if pake_primitive is a supported PAKE
 *         primitive.
 */
#define PSA_PAKE_PRIMITIVE_GET_FAMILY(pake_primitive) \
    /* specification-defined value */

/**
 * @brief Extract the bit-size from a PAKE primitive.
 *
 * @param pake_primitive A PAKE primitive: a value of type psa_pake_primitive_t.
 *
 * @return The PAKE primitive bit-size, if pake_primitive is a supported PAKE
 *         primitive.
 */
#define PSA_PAKE_PRIMITIVE_GET_BITS(pake_primitive) \
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
 * @brief A key confirmation value that indicates an confirmed key in a PAKE
 *        cipher suite.
 */
#define PSA_PAKE_CONFIRMED_KEY 0

/**
 * @brief A key confirmation value that indicates an unconfirmed key in a PAKE
 *        cipher suite.
 */
#define PSA_PAKE_UNCONFIRMED_KEY 1

/**
 * @brief Retrieve the key confirmation from a PAKE cipher suite.
 *
 * @param cipher_suite The cipher suite object to query.
 *
 * @return A key confirmation value: either PSA_PAKE_CONFIRMED_KEY or
 *         PSA_PAKE_UNCONFIRMED_KEY.
 */
uint32_t psa_pake_cs_get_key_confirmation(const psa_pake_cipher_suite_t* cipher_suite);

/**
 * @brief Declare the key confirmation from a PAKE cipher suite.
 *
 * @param cipher_suite     The cipher suite object to write to.
 * @param key_confirmation The key confirmation value to write: either
 *                         PSA_PAKE_CONFIRMED_KEY or PSA_PAKE_UNCONFIRMED_KEY.
 */
void psa_pake_cs_set_key_confirmation(psa_pake_cipher_suite_t* cipher_suite,
                                      uint32_t key_confirmation);

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
 * @brief The key confirmation value.
 */
#define PSA_PAKE_STEP_CONFIRM ((psa_pake_step_t)0x04)

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
 * @brief Setup a password-authenticated key exchange.
 *
 * @param operation    The operation object to set up.
 * @param password_key Identifier of the key holding the password or a value
 *                     derived from the password.
 * @param cipher_suite The cipher suite to use.
 */
psa_status_t psa_pake_setup(psa_pake_operation_t *operation,
                            psa_key_id_t password_key,
                            const psa_pake_cipher_suite_t *cipher_suite);

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
 * @brief Set the context data for a password-authenticated key exchange.
 *
 * @param operation   Active PAKE operation.
 * @param context     The peer's ID to authenticate.
 * @param context_len Size of the context buffer in bytes.
 */
psa_status_t psa_pake_set_context(psa_pake_operation_t *operation,
                                  const uint8_t *context,
                                  size_t context_len);

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
 * @brief Extract the shared secret from the PAKE as a key.
 *
 * @param operation  Active PAKE operation.
 * @param attributes The attributes for the new key.
 * @param key        On success, an identifier for the newly created key.
 */
psa_status_t psa_pake_get_shared_key(psa_pake_operation_t *operation,
                                     const psa_key_attributes_t * attributes,
                                     psa_key_id_t * key);

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

/**
 * @brief Macro to build the Password-authenticated key exchange by juggling
 *        (J-PAKE) algorithm.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return A J-PAKE algorithm, parameterized by a specific hash.
 */
#define PSA_ALG_JPAKE(hash_alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a J-PAKE algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a J-PAKE algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_JPAKE(alg) /* specification-defined value */

/**
 * @brief SPAKE2+ key pair: both the prover and verifier key.
 *
 * @param curve A value of type psa_ecc_family_t that identifies the Elliptic
 *              curve family to be used.
 */
#define PSA_KEY_TYPE_SPAKE2P_KEY_PAIR(curve) /* specification-defined value */

/**
 * @brief SPAKE2+ public key: the verifier key.
 *
 * @param curve A value of type psa_ecc_family_t that identifies the Elliptic
 *              curve family to be used.
 */
#define PSA_KEY_TYPE_SPAKE2P_PUBLIC_KEY(curve) \
    /* specification-defined value */

/**
 * @brief Whether a key type is a SPAKE2+ key, either a key pair or a public
 *        key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SPAKE2P(type) /* specification-defined value */

/**
 * @brief Whether a key type is a SPAKE2+ key pair.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SPAKE2P_KEY_PAIR(type) \
    /* specification-defined value */

/**
 * @brief Whether a key type is a SPAKE2+ public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SPAKE2P_PUBLIC_KEY(type) \
    /* specification-defined value */

/**
 * @brief Extract the curve family from a SPAKE2+ key type.
 *
 * @param type A SPAKE2+ key type: a value of type psa_key_type_t such that
 *             PSA_KEY_TYPE_IS_SPAKE2P(type) is true.
 *
 * @return The elliptic curve family id, if type is a supported SPAKE2+ key.
 */
#define PSA_KEY_TYPE_SPAKE2P_GET_FAMILY(type) /* specification-defined value */

/**
 * @brief Macro to build the SPAKE2+ algorithm, using HMAC-based key
 *        confirmation.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return A SPAKE2+ algorithm, using HMAC for key confirmation, parameterized
 *         by a specific hash.
 */
#define PSA_ALG_SPAKE2P_HMAC(hash_alg) /* specification-defined value */

/**
 * @brief Macro to build the SPAKE2+ algorithm, using CMAC-based key
 *        confirmation.
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return A SPAKE2+ algorithm, using CMAC for key confirmation, parameterized
 *         by a specific hash.
 */
#define PSA_ALG_SPAKE2P_CMAC(hash_alg) /* specification-defined value */

/**
 * @brief The SPAKE2+ algorithm, as used by the Matter v1 specification.
 */
#define PSA_ALG_SPAKE2P_MATTER ((psa_algoirithm_t)0x0A000609)

/**
 * @brief Whether the specified algorithm is a SPAKE2+ algorithm.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a SPAKE2+ algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_SPAKE2P(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a SPAKE2+ algorithm that uses a
 *        HMAC-based key confirmation.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a SPAKE2+ algorithm that uses a HMAC-based key
 *         confirmation, 0 otherwise.
 */
#define PSA_ALG_IS_SPAKE2P_HMAC(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is a SPAKE2+ algorithm that uses a
 *        CMAC-based key confirmation.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a SPAKE2+ algorithm that uses a CMAC-based key
 *         confirmation, 0 otherwise.
 */
#define PSA_ALG_IS_SPAKE2P_CMAC(alg) /* specification-defined value */
