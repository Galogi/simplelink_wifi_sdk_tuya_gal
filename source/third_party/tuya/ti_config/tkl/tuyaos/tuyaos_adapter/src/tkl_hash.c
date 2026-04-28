/**
 * @file tkl_hash.c
 * @brief This file acts as the bridge between Tuya's abstract hash interface and the actual mbedTLS library.
 */

// --- BEGIN: user defines and implements ---
#include "tkl_hash.h"           // Include Tuya's Hash Interface definition
#include "tuya_error_code.h"    // Include Tuya's standard error codes (OPRT_OK, etc.)
#include <string.h>             // Include standard string manipulation functions
#include <stdlib.h>             // Include standard library for malloc/free

// Include mbedTLS headers for the actual cryptographic algorithms
#include <mbedtls/md5.h>        // Driver for MD5 hashing
#include <mbedtls/sha1.h>       // Driver for SHA1 hashing
#include <mbedtls/sha256.h>     // Driver for SHA256 hashing
// --- END: user defines and implements ---

// ============================================================
//                          SHA256
// ============================================================

/**
 * @brief This function allocates and initializes a sha256 context.
 */
OPERATE_RET tkl_sha256_create_init(TKL_HASH_HANDLE *ctx)
{
    // 1. Validate input parameters
    if (ctx == NULL) {                                  // Check if the pointer for returning the handle is valid
        return OPRT_INVALID_PARM;                       // Return error if the input pointer is NULL
    }

    // 2. Allocate memory for the actual mbedTLS context struct
    // We cast the void* return of malloc to a specific mbedtls_sha256_context*
    mbedtls_sha256_context *mbed_ctx = (mbedtls_sha256_context *)malloc(sizeof(mbedtls_sha256_context)); // Allocate heap memory
    
    if (mbed_ctx == NULL) {                             // Check if the allocation succeeded
        return OPRT_MALLOC_FAILED;                      // Return system out-of-memory error if NULL
    }

    // 3. Initialize the allocated memory to zero/safe state
    mbedtls_sha256_init(mbed_ctx);                      // Initialize the mbedTLS struct fields

    // 4. Start the SHA256 calculation
    // Second argument '0' selects SHA-256 mode (passing '1' would select SHA-224)
    int ret = mbedtls_sha256_starts(mbed_ctx, 0);   // Start the engine. Returns 0 on success
    
    if (ret != 0) {                                     // Check if the library returned an error
        mbedtls_sha256_free(mbed_ctx);                  // Clean up internal mbedTLS resources
        free(mbed_ctx);                                 // Free the memory we just allocated
        return OPRT_COM_ERROR;                          // Return a generic communication/internal error
    }

    // 5. Save the address of our new memory into the user's handle pointer
    *ctx = (TKL_HASH_HANDLE)mbed_ctx;                   // Dereference 'ctx' to write the address
    return OPRT_OK;                                     // Return success
}

/**
 * @brief This function cleans up and frees a sha256 context.
 */
OPERATE_RET tkl_sha256_free(TKL_HASH_HANDLE ctx)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL) {                                  // Check if the handle passed in is valid
        return OPRT_INVALID_PARM;                       // Return error if handle is NULL
    }

    // 1. Cast the generic Tuya handle back to our specific mbedTLS struct pointer
    mbedtls_sha256_context *mbed_ctx = (mbedtls_sha256_context *)ctx; // Cast generic pointer to typed pointer

    // 2. Free any internal buffers mbedTLS might have allocated
    mbedtls_sha256_free(mbed_ctx);                      // Release mbedTLS internal memory

    // 3. Free the actual struct memory we malloc'd in create_init
    free(mbed_ctx);                                     // Release the heap memory back to the system

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function resets the context to start a new SHA256 calculation.
 */
OPERATE_RET tkl_sha256_starts_ret(TKL_HASH_HANDLE ctx, int32_t is224)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL) {                                  // Check if the handle is valid
        return OPRT_INVALID_PARM;                       // Return error if invalid
    }

    // Cast the generic handle back to the mbedTLS struct
    mbedtls_sha256_context *mbed_ctx = (mbedtls_sha256_context *)ctx; // Re-create the typed pointer

    // Call the library start function (resets internal counters)
    // is224: 0 = SHA-256, 1 = SHA-224
    int ret = mbedtls_sha256_starts(mbed_ctx, is224); // Perform the reset operation

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function feeds data chunks into the SHA256 calculator.
 */
OPERATE_RET tkl_sha256_update_ret(TKL_HASH_HANDLE ctx, const uint8_t *input, size_t ilen)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL || input == NULL) {                 // Validate context and input buffer
        return OPRT_INVALID_PARM;                       // Return error if any are NULL
    }

    mbedtls_sha256_context *mbed_ctx = (mbedtls_sha256_context *)ctx; // Cast to specific pointer

    // Feed the data bytes into the hash algorithm
    int ret = mbedtls_sha256_update(mbed_ctx, input, ilen); // Process 'ilen' bytes from 'input'

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function calculates the final hash and outputs the 32-byte result.
 */
OPERATE_RET tkl_sha256_finish_ret(TKL_HASH_HANDLE ctx, uint8_t output[32])
{
    // --- BEGIN: user implements ---
    if (ctx == NULL || output == NULL) {                // Validate context and output buffer
        return OPRT_INVALID_PARM;                       // Return error if any are NULL
    }

    mbedtls_sha256_context *mbed_ctx = (mbedtls_sha256_context *)ctx; // Cast to specific pointer

    // Finalize the calculation and write to 'output'
    int ret = mbedtls_sha256_finish(mbed_ctx, output); // Generate the final hash

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

// ============================================================
//                          MD5
// ============================================================

/**
 * @brief This function allocates and initializes an MD5 context.
 */
OPERATE_RET tkl_md5_create_init(TKL_HASH_HANDLE *ctx)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL) {                                  // Validate input pointer
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    // Allocate memory specifically for an MD5 context
    mbedtls_md5_context *mbed_ctx = (mbedtls_md5_context *)malloc(sizeof(mbedtls_md5_context)); // Heap allocation

    if (mbed_ctx == NULL) {                             // Check allocation success
        return OPRT_MALLOC_FAILED;                      // Return error if out of memory
    }

    mbedtls_md5_init(mbed_ctx);                         // Initialize the MD5 struct

    // Start the MD5 engine (Note: MD5 has no "mode" parameter like SHA256)
    int ret = mbedtls_md5_starts(mbed_ctx);         // Start the calculation
    
    if (ret != 0) {                                     // Check for start errors
        mbedtls_md5_free(mbed_ctx);                     // Clean up internal resources
        free(mbed_ctx);                                 // Free the heap memory
        return OPRT_COM_ERROR;                          // Return error code
    }

    *ctx = (TKL_HASH_HANDLE)mbed_ctx;                   // Export the pointer to the user
    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function cleans up and frees an MD5 context.
 */
OPERATE_RET tkl_md5_free(TKL_HASH_HANDLE ctx)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL) {                                  // Validate handle
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    mbedtls_md5_context *mbed_ctx = (mbedtls_md5_context *)ctx; // Cast to MD5 pointer

    mbedtls_md5_free(mbed_ctx);                         // Release mbedTLS internal memory
    free(mbed_ctx);                                     // Free wrapper memory

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function resets the MD5 context.
 */
OPERATE_RET tkl_md5_starts_ret(TKL_HASH_HANDLE ctx)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL) {                                  // Validate handle
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    mbedtls_md5_context *mbed_ctx = (mbedtls_md5_context *)ctx; // Cast to MD5 pointer

    int ret = mbedtls_md5_starts(mbed_ctx);         // Restart/Reset MD5 engine

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function feeds data into the MD5 calculator.
 */
OPERATE_RET tkl_md5_update_ret(TKL_HASH_HANDLE ctx, const uint8_t *input, size_t ilen)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL || input == NULL) {                 // Validate inputs
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    mbedtls_md5_context *mbed_ctx = (mbedtls_md5_context *)ctx; // Cast to MD5 pointer

    int ret = mbedtls_md5_update(mbed_ctx, input, ilen); // Feed bytes to MD5

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function finishes the MD5 operation and outputs 16 bytes.
 */
OPERATE_RET tkl_md5_finish_ret(TKL_HASH_HANDLE ctx, uint8_t output[16])
{
    // --- BEGIN: user implements ---
    if (ctx == NULL || output == NULL) {                // Validate inputs
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    mbedtls_md5_context *mbed_ctx = (mbedtls_md5_context *)ctx; // Cast to MD5 pointer

    int ret = mbedtls_md5_finish(mbed_ctx, output); // Generate final 16-byte hash

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

// ============================================================
//                          SHA1
// ============================================================

/**
 * @brief This function allocates and initializes a SHA1 context.
 */
OPERATE_RET tkl_sha1_create_init(TKL_HASH_HANDLE *ctx)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL) {                                  // Validate input pointer
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    // Allocate memory specifically for a SHA1 context
    mbedtls_sha1_context *mbed_ctx = (mbedtls_sha1_context *)malloc(sizeof(mbedtls_sha1_context)); // Heap allocation

    if (mbed_ctx == NULL) {                             // Check allocation success
        return OPRT_MALLOC_FAILED;                      // Return error if out of memory
    }

    mbedtls_sha1_init(mbed_ctx);                        // Initialize the SHA1 struct

    int ret = mbedtls_sha1_starts(mbed_ctx);        // Start the SHA1 engine
    
    if (ret != 0) {                                     // Check for start errors
        mbedtls_sha1_free(mbed_ctx);                    // Clean up internal resources
        free(mbed_ctx);                                 // Free the heap memory
        return OPRT_COM_ERROR;                          // Return error code
    }

    *ctx = (TKL_HASH_HANDLE)mbed_ctx;                   // Export the pointer to the user
    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function cleans up and frees a SHA1 context.
 */
OPERATE_RET tkl_sha1_free(TKL_HASH_HANDLE ctx)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL) {                                  // Validate handle
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    mbedtls_sha1_context *mbed_ctx = (mbedtls_sha1_context *)ctx; // Cast to SHA1 pointer

    mbedtls_sha1_free(mbed_ctx);                        // Release mbedTLS internal memory
    free(mbed_ctx);                                     // Free wrapper memory

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function resets the SHA1 context.
 */
OPERATE_RET tkl_sha1_starts_ret(TKL_HASH_HANDLE ctx)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL) {                                  // Validate handle
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    mbedtls_sha1_context *mbed_ctx = (mbedtls_sha1_context *)ctx; // Cast to SHA1 pointer

    int ret = mbedtls_sha1_starts(mbed_ctx);        // Restart/Reset SHA1 engine

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function feeds data into the SHA1 calculator.
 */
OPERATE_RET tkl_sha1_update_ret(TKL_HASH_HANDLE ctx, const uint8_t *input, size_t ilen)
{
    // --- BEGIN: user implements ---
    if (ctx == NULL || input == NULL) {                 // Validate inputs
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    mbedtls_sha1_context *mbed_ctx = (mbedtls_sha1_context *)ctx; // Cast to SHA1 pointer

    int ret = mbedtls_sha1_update(mbed_ctx, input, ilen); // Feed bytes to SHA1

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}

/**
 * @brief This function finishes the SHA1 operation and outputs 20 bytes.
 */
OPERATE_RET tkl_sha1_finish_ret(TKL_HASH_HANDLE ctx, uint8_t output[20])
{
    // --- BEGIN: user implements ---
    if (ctx == NULL || output == NULL) {                // Validate inputs
        return OPRT_INVALID_PARM;                       // Return error if NULL
    }

    mbedtls_sha1_context *mbed_ctx = (mbedtls_sha1_context *)ctx; // Cast to SHA1 pointer

    int ret = mbedtls_sha1_finish(mbed_ctx, output); // Generate final 20-byte hash

    if (ret != 0) {                                     // Check for library errors
        return OPRT_COM_ERROR;                          // Return error code
    }

    return OPRT_OK;                                     // Return success
    // --- END: user implements ---
}