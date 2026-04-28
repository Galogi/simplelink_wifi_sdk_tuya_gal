// SPDX-FileCopyrightText: Copyright 2018-2025 Arm Limited and/or its affiliates <open-source-office@arm.com>
// SPDX-License-Identifier: Apache-2.0

/* This file contains reference definitions for implementation of the
 * PSA Certified Crypto API v1.3 PQC Extension beta
 *
 * These definitions must be embedded in, or included by, psa/crypto.h
 */


/**
 * @brief The SHA-256/192 message digest algorithm.
 */
#define PSA_ALG_SHA_256_192 ((psa_algorithm_t)0x0200000E)

/**
 * @brief The SHAKE128/256 message digest algorithm.
 */
#define PSA_ALG_SHAKE128_256 ((psa_algorithm_t)0x02000016)

/**
 * @brief The SHAKE256/192 message digest algorithm.
 */
#define PSA_ALG_SHAKE256_192 ((psa_algorithm_t)0x02000017)

/**
 * @brief The SHAKE256/256 message digest algorithm.
 */
#define PSA_ALG_SHAKE256_256 ((psa_algorithm_t)0x02000018)

/**
 * @brief ML-KEM key pair: both the decapsulation and encapsulation key.
 */
#define PSA_KEY_TYPE_ML_KEM_KEY_PAIR ((psa_key_type_t)0x7004)

/**
 * @brief ML-KEM public (encapsulation) key.
 */
#define PSA_KEY_TYPE_ML_KEM_PUBLIC_KEY ((psa_key_type_t)0x4004)

/**
 * @brief Whether a key type is an ML-DSA key, either a key pair or a public
 *        key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ML_KEM(type) /* specification-defined value */

/**
 * @brief Module Lattice-based key-encapsulation mechanism (ML-KEM).
 */
#define PSA_ALG_ML_KEM ((psa_algorithm_t)0x0c000200)

/**
 * @brief ML-DSA key pair: both the private and public key.
 */
#define PSA_KEY_TYPE_ML_DSA_KEY_PAIR ((psa_key_type_t)0x7002)

/**
 * @brief ML-DSA public key.
 */
#define PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY ((psa_key_type_t)0x4002)

/**
 * @brief Whether a key type is an ML-DSA key, either a key pair or a public
 *        key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_ML_DSA(type) /* specification-defined value */

/**
 * @brief Module lattice-based digital signature algorithm without pre-hashing
 *        (ML-DSA).
 */
#define PSA_ALG_ML_DSA ((psa_algorithm_t) 0x06004400)

/**
 * @brief Deterministic module lattice-based digital signature algorithm without
 *        pre-hashing (ML-DSA).
 */
#define PSA_ALG_DETERMINISTIC_ML_DSA ((psa_algorithm_t) 0x06004500)

/**
 * @brief Module lattice-based digital signature algorithm with pre-hashing
 *        (HashML-DSA).
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding HashML-DSA signature algorithm, using hash_alg to
 *         pre-hash the message.
 */
#define PSA_ALG_HASH_ML_DSA(hash_alg) /* specification-defined value */

/**
 * @brief Deterministic module lattice-based digital signature algorithm with
 *        pre-hashing (HashML-DSA).
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding deterministic HashML-DSA signature algorithm, using
 *         hash_alg to pre-hash the message.
 */
#define PSA_ALG_DETERMINISTIC_HASH_ML_DSA(hash_alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is ML-DSA, without pre-hashing.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a pure ML-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_ML_DSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is HashML-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a HashML-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HASH_ML_DSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is deterministic HashML-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a deterministic HashML-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_DETERMINISTIC_HASH_ML_DSA(alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is hedged HashML-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a hedged HashML-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HEDGED_HASH_ML_DSA(alg) /* specification-defined value */

/**
 * @brief The type of identifiers of a Stateless hash-based DSA parameter set.
 */
typedef uint8_t psa_slh_dsa_family_t;

/**
 * @brief SLH-DSA key pair: both the private key and public key.
 *
 * @param set A value of type psa_slh_dsa_family_t that identifies the SLH-DSA
 *            parameter-set family to be used.
 */
#define PSA_KEY_TYPE_SLH_DSA_KEY_PAIR(set) /* specification-defined value */

/**
 * @brief SLH-DSA public key.
 *
 * @param set A value of type psa_slh_dsa_family_t that identifies the SLH-DSA
 *            parameter-set family to be used.
 */
#define PSA_KEY_TYPE_SLH_DSA_PUBLIC_KEY(set) /* specification-defined value */

/**
 * @brief SLH-DSA family for the SLH-DSA-SHA2-NNNs parameter sets.
 */
#define PSA_SLH_DSA_FAMILY_SHA2_S ((psa_slh_dsa_family_t) 0x02)

/**
 * @brief SLH-DSA family for the SLH-DSA-SHA2-NNNf parameter sets.
 */
#define PSA_SLH_DSA_FAMILY_SHA2_F ((psa_slh_dsa_family_t) 0x04)

/**
 * @brief SLH-DSA family for the SLH-DSA-SHAKE-NNNs parameter sets.
 */
#define PSA_SLH_DSA_FAMILY_SHAKE_S ((psa_slh_dsa_family_t) 0x0b)

/**
 * @brief SLH-DSA family for the SLH-DSA-SHAKE-NNNf parameter sets.
 */
#define PSA_SLH_DSA_FAMILY_SHAKE_F ((psa_slh_dsa_family_t) 0x0d)

/**
 * @brief Whether a key type is an SLH-DSA key, either a key pair or a public
 *        key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SLH_DSA(type) /* specification-defined value */

/**
 * @brief Whether a key type is an SLH-DSA key pair.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SLH_DSA_KEY_PAIR(type) \
    /* specification-defined value */

/**
 * @brief Whether a key type is an SLH-DSA public key.
 *
 * @param type A key type: a value of type psa_key_type_t.
 */
#define PSA_KEY_TYPE_IS_SLH_DSA_PUBLIC_KEY(type) \
    /* specification-defined value */

/**
 * @brief Extract the parameter-set family from an SLH-DSA key type.
 *
 * @param type An SLH-DSA key type: a value of type psa_key_type_t such that
 *             PSA_KEY_TYPE_IS_SLH_DSA(type) is true.
 *
 * @return The SLH-DSA parameter-set family id, if type is a supported SLH-DSA
 *         key.
 */
#define PSA_KEY_TYPE_SLH_DSA_GET_FAMILY(type) /* specification-defined value */

/**
 * @brief Stateless hash-based digital signature algorithm without pre-hashing
 *        (SLH-DSA).
 */
#define PSA_ALG_SLH_DSA ((psa_algorithm_t) 0x06004000)

/**
 * @brief Deterministic stateless hash-based digital signature algorithm without
 *        pre-hashing (SLH-DSA).
 */
#define PSA_ALG_DETERMINISTIC_SLH_DSA ((psa_algorithm_t) 0x06004100)

/**
 * @brief Stateless hash-based digital signature algorithm with pre-hashing
 *        (HashSLH-DSA).
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding HashSLH-DSA signature algorithm, using hash_alg to
 *         pre-hash the message.
 */
#define PSA_ALG_HASH_SLH_DSA(hash_alg) /* specification-defined value */

/**
 * @brief Deterministic stateless hash-based digital signature algorithm with
 *        pre-hashing (HashSLH-DSA).
 *
 * @param hash_alg A hash algorithm: a value of type psa_algorithm_t such that
 *                 PSA_ALG_IS_HASH(hash_alg) is true.
 *
 * @return The corresponding deterministic HashSLH-DSA signature algorithm,
 *         using hash_alg to pre-hash the message.
 */
#define PSA_ALG_DETERMINISTIC_HASH_SLH_DSA(hash_alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is SLH-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is an SLH-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_SLH_DSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is HashSLH-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a HashSLH-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HASH_SLH_DSA(alg) /* specification-defined value */

/**
 * @brief Whether the specified algorithm is deterministic HashSLH-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a deterministic HashSLH-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_DETERMINISTIC_HASH_SLH_DSA(alg) \
    /* specification-defined value */

/**
 * @brief Whether the specified algorithm is hedged HashSLH-DSA.
 *
 * @param alg An algorithm identifier: a value of type psa_algorithm_t.
 *
 * @return 1 if alg is a hedged HashSLH-DSA algorithm, 0 otherwise.
 */
#define PSA_ALG_IS_HEDGED_HASH_SLH_DSA(alg) /* specification-defined value */

/**
 * @brief Leighton-Micali Signatures (LMS) public key.
 */
#define PSA_KEY_TYPE_LMS_PUBLIC_KEY ((psa_key_type_t)0x4007)

/**
 * @brief Hierarchical Signature Scheme (HSS) public key.
 */
#define PSA_KEY_TYPE_HSS_PUBLIC_KEY ((psa_key_type_t)0x4008)

/**
 * @brief Leighton-Micali Signatures (LMS) signature algorithm.
 */
#define PSA_ALG_LMS ((psa_algorithm_t) 0x06004800)

/**
 * @brief Hierarchical Signature Scheme (HSS) signature algorithm.
 */
#define PSA_ALG_HSS ((psa_algorithm_t) 0x06004900)

/**
 * @brief eXtended Merkle Signature Scheme (XMSS) public key.
 */
#define PSA_KEY_TYPE_XMSS_PUBLIC_KEY ((psa_key_type_t)0x400B)

/**
 * @brief Multi-tree eXtended Merkle Signature Scheme (XMSS^MT) public key.
 */
#define PSA_KEY_TYPE_XMSS_MT_PUBLIC_KEY ((psa_key_type_t)0x400D)

/**
 * @brief eXtended Merkle Signature Scheme (XMSS) signature algorithm.
 */
#define PSA_ALG_XMSS ((psa_algorithm_t) 0x06004A00)

/**
 * @brief Multi-tree eXtended Merkle Signature Scheme (XMSS^MT) signature
 *        algorithm.
 */
#define PSA_ALG_XMSS_MT ((psa_algorithm_t) 0x06004B00)
