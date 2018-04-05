#pragma once

#include "ipb_clib.h"
#include "ReferenceCountedBase.h"


class CSignalingLine :public ReferenceCountedBase
{
public:
	typedef SmartPointer<CSignalingLine>SIGNALINGLINE_SMART_PTR;
	typedef std::map<uint32_t,SIGNALINGLINE_SMART_PTR>LineMap_T;
public:
	uint32_t	nLineNumber;
	std::string	szLineLabel;
	std::string	szFQLineName;
	std::string	szLineDN;


protected:
	~CSignalingLine(){};
public:
	CSignalingLine():nLineNumber(0){};
};

class CSignalingCall:public ReferenceCountedBase
{
public:
	typedef SmartPointer<CSignalingCall>SIGNALINGCALL_SMART_PTR ;
	typedef std::map<uint32_t,SIGNALINGCALL_SMART_PTR>CallMap_T;
	typedef enum tagCallStateEnum{SIGNALING_CALL_STATE_CREATED,SIGNALING_CALL_STATE_OFFHOOK,SIGNALING_CALL_STATE_CONNECTING,SIGNALING_CALL_STATE_DISCONNECTED,SIGNALING_CALL_STATE_ALERTING,SIGNALING_CALL_STATE_RINGIN,SIGNALING_CALL_STATE_CONNECTED,SIGNALING_CALL_STATE_ERROR,SIGNALING_CALL_STATE_HELD,SIGNALING_CALL_STATE_REMOTE,SIGNALING_CALL_STATE_IGNORED}CallStateEnum;
	typedef enum tagCallErrorEnum{SIGNALING_CALL_ERROR_NONE,SIGNALING_CALL_ERROR_CONGESTION,SIGNALING_CALL_ERROR_INVALID_NUMBER,SIGNALING_CALL_ERROR_BUSY,SIGNALING_CALL_ERROR_TIMEOUT}CallErrorEnum;
	typedef enum tagDirectionEnum{Unknown,Outbound,Inbound,Remote}DirectionEnum;
	typedef enum tagCodec{CodecNotSet=-1,AudioCodecG711a,AudioCodecG711u,AudioCodecG729,AudioCodecG729B,AudioCodecG729a, AudioCodecG722}CodecsEnum;
	typedef enum tagIPVersion{IPV4,IPV6}IpVersion;
	typedef enum tagRingerState{RingerStateOff,RingerStateOn,RingerStateInside,RingerSilent}RingerState;

public:
	uint32_t					nCallRef;
	uint32_t					nLineNumber;
	std::string			szDialedNumber;
	std::string			szCallerName;
	std::string			szCallerNumber;
	bool					bPrivate;
	CallErrorEnum			nCallError;
	CallStateEnum			nCallState;
	DirectionEnum			nDirection;
	CodecsEnum				nTxCodec;
	CodecsEnum				nRXCodec;
	std::string			szFarEndToIP;
	std::string			szFarEndFromIP;
	unsigned short			nFarEndToPort;
	unsigned short			nFarEndFromPort;
	std::string			szNearEndIP;
	unsigned short			nNearEndPort;
	unsigned long			nTxPacketSize;
	unsigned long			nRxPacketSize;
	IpVersion				nIpVersion;
	RingerState				nRingerState;
	bool					bMuted;
	bool					bActiveRX;
	bool					bActiveTX;
	uint32_t					nDSCPTXValue;

	std::recursive_mutex	m_Lock;

protected:
	~CSignalingCall(){};
public:
	CSignalingCall():nLineNumber(0),bPrivate(false),nCallError(SIGNALING_CALL_ERROR_NONE),nCallState(SIGNALING_CALL_STATE_CREATED),nDirection(Unknown),nTxCodec(CodecNotSet),\
			nRXCodec(CodecNotSet),nFarEndToPort(0),nFarEndFromPort(0),nNearEndPort(0),nIpVersion(IPV4),nTxPacketSize(20),nRxPacketSize(20),nRingerState(RingerStateOff),bMuted(false),
					 bActiveRX(false),bActiveTX(false),nCallRef(0),nDSCPTXValue(-1){};
};

class CSignalingSpeedDial:public ReferenceCountedBase
{
public:
	typedef SmartPointer<CSignalingSpeedDial>SPEEDDIAL_SMART_PTR;
	typedef std::map<uint32_t,SPEEDDIAL_SMART_PTR>SpeedDialMap_T;
public:
	uint32_t			nSpeedDialNumber;
	std::string	szName;
	std::string	szNumber;
protected:
	~CSignalingSpeedDial(){};
public:
	CSignalingSpeedDial():nSpeedDialNumber(0){};
};

typedef struct _PlayToneInfo
{
	enum	PhoneToneTypes{	PT_STOP=-1,PT_NONE=0,PT_DTMF1,PT_DTMF2,PT_DTMF3,PT_DTMF4,PT_DTMF5,PT_DTMF6,PT_DTMF7,PT_DTMF8,PT_DTMF9,PT_DTMF0,PT_DTMFPOUND,PT_DTMFSTAR,PT_INSIDE_DIAL,PT_OUTSIDE_DIAL,PT_LINE_BUSY,PT_ALERTING,PT_REORDER,PT_CALLWAITING,PT_HOLDTONE, PT_CALLBACK};

	PhoneToneTypes	nToneType;
	bool			bContinuous;
	uint32_t			nLineId;
	uint32_t			nCallId;
	bool			bPlayInStream;
	_PlayToneInfo	():nToneType(PT_NONE),bContinuous(false),nLineId(-1),nCallId(-1),bPlayInStream(false){};

}PlayToneInfo,*PPlayToneInfo;

typedef struct _SystemMessageInfo
{
	enum MessageStateType{MST_UNKNOWN=-1,MST_DISPLAY=0,MST_CLEAR};

	MessageStateType 	nState;
	uint32_t				nPriority;
	uint32_t				nTimeOut;
	uint32_t				nCallRef;
	uint32_t				nLineID;
	union
	{
		char	szData[256];
		uint32_t	szTokenId;

	}DisplayData;
	_SystemMessageInfo():nState(MST_UNKNOWN),nPriority(-1),nTimeOut(-1),nCallRef(0),nLineID(0)
	{
		memset(DisplayData.szData,0,sizeof(DisplayData.szData));
	};

	_SystemMessageInfo(const _SystemMessageInfo& rhs):nState(MST_UNKNOWN),nPriority(-1),nTimeOut(-1),nCallRef(0),nLineID(0)
	{
		nState=rhs.nState;
		nPriority=rhs.nPriority;
		nTimeOut=rhs.nTimeOut;
		nCallRef=rhs.nCallRef;
		nLineID=rhs.nLineID;
		memcpy(DisplayData.szData,rhs.DisplayData.szData,sizeof(DisplayData.szData));

	}
}SystemMessageInfo,*PSystemMessageInfo;

class ISignaling:public ReferenceCountedBase
{
public:
	typedef SmartPointer<ISignaling>SIGNALING_SMART_PTR;
public:
	enum	StackState{Disposing=-1,Disconnected=0,Disconnecting,Connecting,Connected,Registering,Registered};
	enum	ErrorCode {Error_Success=0,Error_InvalidState=1,Error_InternalError,Error_SocketError,Error_MissingData,Error_NotImplemented,Error_NotRegistered,Error_InvalidLine,Error_InvalidCall,Error_InvalidDigit,Error_PendingCall,Error_TimedOut};
	enum	SignalingEventType{CONNECTION_EVENT,REGISTRATION_EVENT,PHONE_EVENT,LINE_EVENT,CALL_EVENT};
	enum	ConnectionSubEventType{ConnectionEstablished,ConnectionFailure,ConnectionClosed,KeepAliveFailure, ConnectionRestart, ConnectionReset};
	enum	RegistrationSubEventType{RegEventRegistrationFailed,RegEventRegistered,RegEventUnRegistered,RegEventTokenAck,RegEventTokenReject};
	enum	PhoneSubEventType{PhoneSubEventSoftKeyUpdate,PhoneSpeedDialAdded,PhonePlayTone,PlaceCallTimedOut,PhoneSystemMessage,MessageWaiting};
	enum	CallSubEventType{CallSubEvent_Created,CallSubEvent_StateChange,CallSubEvent_Removed,CallSubEvent_InfoUpdate,CallSubEvent_StartMediaRX,CallSubEvent_StartMediaTX,CallSubEvent_StopMediaRX,CallSubEvent_StopMediaTX,CallSubEvent_RingerChange};
	enum	LineSubEventType{LineSubEvent_NewLine,LineSubEvent_ForwardStateChange,LineSubEvent_SetActiveLine};
	enum	PlaceCallOptions{PlaceCallOptionsNone,PlaceCallOptionsSendDigits};
	enum	MediaStatusCodes{MediaStatusOK=0,MediaStatusError=1};

	//typedef boost::function<void (ISignaling *sender,SignalingEventType nEvent,int nSubEvent,void* lpData)>SIGNALLINGCALLBACKFUNCTION_T;
	typedef std::function<void (ISignaling *sender,SignalingEventType nEvent,int nSubEvent,void* lpData)>SIGNALLINGCALLBACKFUNCTION_T;
protected:
	virtual									~ISignaling(){};
public:
   static std::string DecodeStackState(StackState state)
   {
      switch (state)
      {
         case Disposing:      return "Disposing";
         case Disconnected:   return "Disconnected";
         case Disconnecting:  return "Disconnecting";
         case Connecting:     return "Connecting";
         case Connected:      return "Connected";
         case Registering:    return "Registering";
         case Registered:     return "Registered";
         default:             return "Unknown state";
      }
   }

   static std::string DecodeError(ErrorCode err)
   {
      switch (err)
      {
      case Error_Success:            return "Error_Success";
      case Error_InvalidState:    return "Error_InvalidState";
      case Error_InternalError:   return "Error_InternalError";
      case Error_SocketError:     return "Error_SocketError";
      case Error_MissingData:     return "Error_MissingData";
      case Error_NotImplemented:  return "Error_NotImplemented";
      case Error_NotRegistered:   return "Error_NotRegistered";
      case Error_InvalidLine:     return "Error_InvalidLine";
      case Error_InvalidCall:     return "Error_InvalidCall";
      case Error_InvalidDigit:    return "Error_InvalidDigit";
      case Error_PendingCall:     return "Error_PendingCall";
      case Error_TimedOut:        return "Error_TimedOut";
      default:                    return "Unknown error";
      }
   }

public:
	ISignaling(){};

	virtual ErrorCode					Connect()=0;
	virtual ErrorCode					Disconnect()=0;
	virtual ErrorCode					Register()=0;
   virtual ErrorCode					Unregister() = 0;
   virtual	ErrorCode					SetIPAddress(std::string szAddress)=0;
	virtual void						RegisterCallback(SIGNALLINGCALLBACKFUNCTION_T ) = 0;
	virtual ErrorCode					SetCodecString(std::string szCodecList){return(Error_NotImplemented);};
	virtual ErrorCode					SetDSCPValues(int nDSCPValueSignalling,int nDSCPValueRTP){return(Error_Success);};
	virtual std::string				ProxyAddress(){return std::string((""));};
	virtual uint32_t						GetLineCount(){return(0);};
	virtual CSignalingLine*				GetLineByIndex(unsigned int nLineID){return(NULL);};
	virtual CSignalingLine*				GetLineByID(int nLineID){return(NULL);};
	virtual uint32_t						GetCallCount(){return(0);};
	virtual CSignalingCall*				GetCallByID(uint32_t){return(NULL);};
	virtual CSignalingCall*				GetCallByIndex(unsigned int nCallID){return(NULL);};
	virtual void						OnStartMediaRX(CSignalingCall *pCall,MediaStatusCodes nStatus){};
	virtual void						OnStartMediaTx(CSignalingCall *pCall,MediaStatusCodes nStatus){};
	virtual ErrorCode					PlaceCall(std::string szNumber,unsigned int nLineID,int nSignalingOptions){return(Error_NotImplemented);};
	virtual ErrorCode					EndCall(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					AnswerCall(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					IgnoreCall(unsigned int nCallRef,bool bLocalIgnore){return(Error_NotImplemented);};
	virtual ErrorCode					HoldCall(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					ResumeCall(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					InitiateTransfer(unsigned int nCallRef,std::string szNumber){return(Error_NotImplemented);};
	virtual ErrorCode					CompleteTransfer(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					AbortTransfer(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					InitiateConference(unsigned int nCallRef,std::string szNumber){return(Error_NotImplemented);};
	virtual ErrorCode					CompleteConference(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					AbortConference(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					SendSoftKeyEvent(unsigned int nEvent, unsigned int nLineNumber, unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode             ParkCall(unsigned int nCallRef){return(Error_NotImplemented);};
	virtual ErrorCode					SendDigit(unsigned int nCallRef,char nDigit){return(Error_NotImplemented);};
	virtual CSignalingSpeedDial*  GetSpeedDialByID(uint32_t nId){return(NULL);};
	virtual CSignalingSpeedDial*  GetSpeedDialByIndex(int nIndex){return(NULL);};
	virtual uint32_t              GetSpeedDialCount(){return(0);};
	virtual std::string           GetServer(){return(std::string(""));};
   virtual ISignaling::StackState        GetStackState() { return Disposing; }
   virtual ISignaling::ErrorCode Reset() { return(Error_NotImplemented); };
	virtual ErrorCode					RetrieveVoiceMail(int nLineId){return(Error_NotImplemented);};
   
   virtual ISignaling::ErrorCode SendStationRegisterTokenReq() {return(Error_NotImplemented);};

};



