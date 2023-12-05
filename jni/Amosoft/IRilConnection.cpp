#ifndef IRILCONNECTION_H
#define IRILCONNECTION_H
#include <iostream>
#include <functional>

#include <string>
#include <iostream>
#include <regex>

#include "models/AuthModel.cpp" 

namespace Amosoft
{
	class IRilConnection
	{
		private:
			#ifdef NDEBUG
			IRilConnection();
			#endif
		

		public:
			#ifdef NDEBUG
			IRilConnection(const AuthModel& authModel_) : authModel(authModel_) {}
			#else
			IRilConnection(){}
			#endif

			virtual ~IRilConnection() = default; // make dtor virtual
			//IRilConnection(IRilConnection&&) = default;  // support moving
			//IRilConnection& operator=(IRilConnection&&) = default;
			//IRilConnection(const IRilConnection&) = default; // support copying
			//IRilConnection& operator=(const IRilConnection&) = default;	


			//Methods
			virtual bool Initialized() = 0;
			
			inline static std::string Imei;
			

		protected:
		
			struct OEMRequestRawHeader {
				unsigned char main_cmd;
				unsigned char sub_cmd;
				unsigned short length;
				unsigned short cmdLength;
			} __attribute__((packed));
			
			bool _initialized = false;
			
			inline static const std::string StrFail = "FAIL";
			inline static const std::string StrOk = "OK";

			#ifdef NDEBUG
			const AuthModel& authModel;
			#endif

			

			typedef enum {
				RIL_E_SUCCESS = 0,
				RIL_E_RADIO_NOT_AVAILABLE = 1,     /* If radio did not start or is resetting */
				RIL_E_GENERIC_FAILURE = 2,
				RIL_E_PASSWORD_INCORRECT = 3,      /* for PIN/PIN2 methods only! */
				RIL_E_SIM_PIN2 = 4,                /* Operation requires SIM PIN2 to be entered */
				RIL_E_SIM_PUK2 = 5,                /* Operation requires SIM PIN2 to be entered */
				RIL_E_REQUEST_NOT_SUPPORTED = 6,
				RIL_E_CANCELLED = 7,
				RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL = 8, /* data ops are not allowed during voice
																call on a Class C GPRS device */
				RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW = 9,  /* data ops are not allowed before device
																registers in network */
				RIL_E_SMS_SEND_FAIL_RETRY = 10,             /* fail to send sms and need retry */
				RIL_E_SIM_ABSENT = 11,                      /* fail to set the location where CDMA subscription
																shall be retrieved because of SIM or RUIM
																card absent */
				RIL_E_SUBSCRIPTION_NOT_AVAILABLE = 12,      /* fail to find CDMA subscription from specified
																location */
				RIL_E_MODE_NOT_SUPPORTED = 13,              /* HW does not support preferred network type */
				RIL_E_FDN_CHECK_FAILURE = 14,               /* command failed because recipient is not on FDN list */
				RIL_E_ILLEGAL_SIM_OR_ME = 15,               /* network selection failed due to
																illegal SIM or ME */
				RIL_E_MISSING_RESOURCE = 16,                /* no logical channel available */
				RIL_E_NO_SUCH_ELEMENT = 17,                  /* application not found on SIM */
				RIL_E_DIAL_MODIFIED_TO_USSD = 18,           /* DIAL request modified to USSD */
				RIL_E_DIAL_MODIFIED_TO_SS = 19,             /* DIAL request modified to SS */
				RIL_E_DIAL_MODIFIED_TO_DIAL = 20,           /* DIAL request modified to DIAL with different
																data */
				RIL_E_USSD_MODIFIED_TO_DIAL = 21,           /* USSD request modified to DIAL */
				RIL_E_USSD_MODIFIED_TO_SS = 22,             /* USSD request modified to SS */
				RIL_E_USSD_MODIFIED_TO_USSD = 23,           /* USSD request modified to different USSD
																request */
				RIL_E_SS_MODIFIED_TO_DIAL = 24,             /* SS request modified to DIAL */
				RIL_E_SS_MODIFIED_TO_USSD = 25,             /* SS request modified to USSD */
				RIL_E_SUBSCRIPTION_NOT_SUPPORTED = 26,      /* Subscription not supported by RIL */
				RIL_E_SS_MODIFIED_TO_SS = 27,               /* SS request modified to different SS request */
				RIL_E_LCE_NOT_SUPPORTED = 36,               /* LCE service not supported(36 in RILConstants.java) */
				RIL_E_NO_MEMORY = 37,                       /* Not sufficient memory to process the request */
				RIL_E_INTERNAL_ERR = 38,                    /* Modem hit unexpected error scenario while handling
																this request */
				RIL_E_SYSTEM_ERR = 39,                      /* Hit platform or system error */
				RIL_E_MODEM_ERR = 40,                       /* Vendor RIL got unexpected or incorrect response
																from modem for this request */
				RIL_E_INVALID_STATE = 41,                   /* Unexpected request for the current state */
				RIL_E_NO_RESOURCES = 42,                    /* Not sufficient resource to process the request */
				RIL_E_SIM_ERR = 43,                         /* Received error from SIM card */
				RIL_E_INVALID_ARGUMENTS = 44,               /* Received invalid arguments in request */
				RIL_E_INVALID_SIM_STATE = 45,               /* Can not process the request in current SIM state */
				RIL_E_INVALID_MODEM_STATE = 46,             /* Can not process the request in current Modem state */
				RIL_E_INVALID_CALL_ID = 47,                 /* Received invalid call id in request */
				RIL_E_NO_SMS_TO_ACK = 48,                   /* ACK received when there is no SMS to ack */
				RIL_E_NETWORK_ERR = 49,                     /* Received error from network */
				RIL_E_REQUEST_RATE_LIMITED = 50,            /* Operation denied due to overly-frequent requests */
				RIL_E_SIM_BUSY = 51,                        /* SIM is busy */
				RIL_E_SIM_FULL = 52,                        /* The target EF is full */
				RIL_E_NETWORK_REJECT = 53,                  /* Request is rejected by network */
				RIL_E_OPERATION_NOT_ALLOWED = 54,           /* Not allowed the request now */
				RIL_E_EMPTY_RECORD = 55,                    /* The request record is empty */
				RIL_E_INVALID_SMS_FORMAT = 56,              /* Invalid sms format */
				RIL_E_ENCODING_ERR = 57,                    /* Message not encoded properly */
				RIL_E_INVALID_SMSC_ADDRESS = 58,            /* SMSC address specified is invalid */
				RIL_E_NO_SUCH_ENTRY = 59,                   /* No such entry present to perform the request */
				RIL_E_NETWORK_NOT_READY = 60,               /* Network is not ready to perform the request */
				RIL_E_NOT_PROVISIONED = 61,                 /* Device doesnot have this value provisioned */
				RIL_E_NO_SUBSCRIPTION = 62,                 /* Device doesnot have subscription */
				RIL_E_NO_NETWORK_FOUND = 63,                /* Network cannot be found */
				RIL_E_DEVICE_IN_USE = 64,                   /* Operation cannot be performed because the device
																is currently in use */
				RIL_E_ABORTED = 65,                         /* Operation aborted */
				RIL_E_INVALID_RESPONSE = 66,                /* Invalid response sent by vendor code */
				// OEM specific error codes. To be used by OEM when they don't want to reveal
				// specific error codes which would be replaced by Generic failure.
				RIL_E_OEM_ERROR_1 = 501,
				RIL_E_OEM_ERROR_2 = 502,
				RIL_E_OEM_ERROR_3 = 503,
				RIL_E_OEM_ERROR_4 = 504,
				RIL_E_OEM_ERROR_5 = 505,
				RIL_E_OEM_ERROR_6 = 506,
				RIL_E_OEM_ERROR_7 = 507,
				RIL_E_OEM_ERROR_8 = 508,
				RIL_E_OEM_ERROR_9 = 509,
				RIL_E_OEM_ERROR_10 = 510,
				RIL_E_OEM_ERROR_11 = 511,
				RIL_E_OEM_ERROR_12 = 512,
				RIL_E_OEM_ERROR_13 = 513,
				RIL_E_OEM_ERROR_14 = 514,
				RIL_E_OEM_ERROR_15 = 515,
				RIL_E_OEM_ERROR_16 = 516,
				RIL_E_OEM_ERROR_17 = 517,
				RIL_E_OEM_ERROR_18 = 518,
				RIL_E_OEM_ERROR_19 = 519,
				RIL_E_OEM_ERROR_20 = 520,
				RIL_E_OEM_ERROR_21 = 521,
				RIL_E_OEM_ERROR_22 = 522,
				RIL_E_OEM_ERROR_23 = 523,
				RIL_E_OEM_ERROR_24 = 524,
				RIL_E_OEM_ERROR_25 = 525
			} RIL_Errno;

			typedef enum
			{
				RIL_CLIENT_ERR_SUCCESS,
				RIL_CLIENT_ERR_AGAIN,
				RIL_CLIENT_ERR_INIT,  		// Client is not initialized
				RIL_CLIENT_ERR_INVAL, 		// Invalid value
				RIL_CLIENT_ERR_CONNECT,		// Connection error
				RIL_CLIENT_ERR_IO,			// IO error
				RIL_CLIENT_ERR_RESOURCE,	// Resource not available
				RIL_CLIENT_ERR_UNKNOWN
			} RilStatus;
 };
}

#endif