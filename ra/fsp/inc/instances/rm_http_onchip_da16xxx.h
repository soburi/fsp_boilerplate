/***********************************************************************************************************************
 * Copyright [2020-2024] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @addtogroup HTTP_ONCHIP_DA16XXX HTTP_ONCHIP_DA16XXX
 * @{
 **********************************************************************************************************************/

#ifndef RM_HTTP_ONCHIP_DA16XXX_H_
#define RM_HTTP_ONCHIP_DA16XXX_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rm_wifi_da16xxx.h"
#include "rm_at_transport_da16xxx.h"
#include "rm_http_onchip_da16xxx_cfg.h"

#define HTTP_ONCHIP_DA16XXX_MAX_ALPN       (3)    ///< Maximum number of ALPNs supported by DA16XXX.
#define HTTP_ONCHIP_DA16XXX_MAX_SNI_LEN    (64)   ///< Maximum length of SNI supported by DA16XXX.
#define HTTP_ONCHIP_DA16XXX_MAX_GET_LEN    (2048) ///< Maximum length of HTTP GET request

/** HTTP Request Method */
typedef enum e_http_onchip_da16xxx_method
{
    HTTP_ONCHIP_DA16XXX_GET  = 0,      ///< Value converted to "get" string
    HTTP_ONCHIP_DA16XXX_POST = 1,      ///< Value converted to "post" string
    HTTP_ONCHIP_DA16XXX_PUT  = 2       ///< Value converted to "put" string
} http_onchip_da16xxx_method_t;

/** HTTP TLS Authentication Level */
typedef enum e_http_onchip_da16xxx_tls_auth
{
    HTTP_ONCHIP_DA16XXX_TLS_VERIFY_NONE     = 0, ///< TLS verification is not used
    HTTP_ONCHIP_DA16XXX_TLS_VERIFY_OPTIONAL = 1, ///< TLS verification is optional
    HTTP_ONCHIP_DA16XXX_TLS_VERIFY_REQUIRED = 2  ///< TLS verification is required
} http_onchip_da16xxx_tls_auth_t;

/** HTTP Request Buffer */
typedef struct st_http_onchip_da16xxx_request
{
    const char                 * p_http_endpoint; ///< Defines the URL to send HTTP requests to
    const char                 * p_request_body;  ///< Pointer to optional buffer for body
    http_onchip_da16xxx_method_t method;          ///< Request method to be used
    uint32_t length;                              ///< Length of optional body buffer
} http_onchip_da16xxx_request_t;

/** HTTP User Buffers */
typedef struct st_http_onchip_da16xxx_buffer
{
    char   * p_request_buffer;         ///< User HTTP request buffer
    uint32_t req_length;               ///< Size of HTTP request buffer
    char   * p_response_buffer;        ///< User HTTP request buffer
    uint32_t resp_length;              ///< Size of HTTP request buffer
} http_onchip_da16xxx_buffer_t;

/** HTTP Configuration */
typedef struct st_http_onchip_da16xxx_cfg
{
    at_transport_da16xxx_instance_t const * p_transport_instance;

    const char * p_alpns[HTTP_ONCHIP_DA16XXX_MAX_ALPN]; ///< Buffer for storing ALPN names
    uint8_t      alpn_count;                            ///< ALPN Protocols count. Max value is 3.
    const char * p_sni_name;                            ///< Pointer to Server Name Indication
    http_onchip_da16xxx_tls_auth_t tls_level;           ///< Set TLS Authentication Level (0, 1, or 2)
    const char * p_root_ca;                             ///< String representing a trusted server root certificate.
    uint32_t     root_ca_size;                          ///< Size associated with root CA Certificate.
    const char * p_client_cert;                         ///< String representing a Client certificate.
    uint32_t     client_cert_size;                      ///< Size associated with Client certificate.
    const char * p_client_private_key;                  ///< String representing Client Private Key.
    uint32_t     private_key_size;                      ///< Size associated with Client Private Key.
} http_onchip_da16xxx_cfg_t;

/** HTTP_ONCHIP_DA16XXX private control block. DO NOT MODIFY. */
typedef struct st_http_onchip_da16xxx_instance_ctrl
{
    uint32_t open;                           ///< Flag to indicate if HTTP has been opened.
    http_onchip_da16xxx_cfg_t const * p_cfg; ///< Pointer to p_cfg for HTTP.
} http_onchip_da16xxx_instance_ctrl_t;

/*******************************************************************************
 *  On Chip HTTP Client Public API definitions
 ********************************************************************************/

/* HTTP public function prototypes */
fsp_err_t RM_HTTP_DA16XXX_Open(http_onchip_da16xxx_instance_ctrl_t   * p_ctrl,
                               http_onchip_da16xxx_cfg_t const * const p_cfg);
fsp_err_t RM_HTTP_DA16XXX_Send(http_onchip_da16xxx_instance_ctrl_t * p_ctrl,
                               http_onchip_da16xxx_request_t       * p_request,
                               http_onchip_da16xxx_buffer_t        * p_buffer);

fsp_err_t RM_HTTP_DA16XXX_Close(http_onchip_da16xxx_instance_ctrl_t * p_ctrl);

#endif                                 /* RM_HTTP_ONCHIP_DA16XXX_H_ */

/*******************************************************************************************************************//**
 * @} (end addtogroup HTTP_ONCHIP_DA16XXX)
 **********************************************************************************************************************/
