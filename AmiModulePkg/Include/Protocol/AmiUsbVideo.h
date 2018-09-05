//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************

/** @file

    USB Video protocol definitions.

**/

#ifndef __AMI_USB_VIDEO_PROTOCOL__H__
#define __AMI_USB_VIDEO_PROTOCOL__H__

// Video Interface Class Code
#define AMI_UVC_CC_VIDEO    0x0E

// Video Interface Subclass Codes
#define AMI_UVC_SC_UNDEFINED            0x00
#define AMI_UVC_SC_VIDEOCONTROL         0x01
#define AMI_UVC_SC_VIDEOSTREAMING       0x02
#define AMI_UVC_SC_VIDEO_INTERFACE_COLLECTION   0x03

// Video Interface Protocol Codes
#define AMI_UVC_PC_PROTOCOL_UNDEFINED   0x00
#define AMI_UVC_PC_PROTOCOL_15          0x01

// Video Class-Specific Descriptor Types
#define AMI_UVC_CS_UNDEFINED            0x20
#define AMI_UVC_CS_DEVICE               0x21
#define AMI_UVC_CS_CONFIGURATION        0x22
#define AMI_UVC_CS_STRING               0x23
#define AMI_UVC_CS_INTERFACE            0x24
#define AMI_UVC_CS_ENDPOINT             0x25

// Video Class-Specific VC Interface Descriptor Subtypes
#define AMI_UVC_VC_DESCRIPTOR_UNDEFINED 0x00
#define AMI_UVC_VC_HEADER               0x01
#define AMI_UVC_VC_INPUT_TERMINAL       0x02
#define AMI_UVC_VC_OUTPUT_TERMINAL      0x03
#define AMI_UVC_VC_SELECTOR_UNIT        0x04
#define AMI_UVC_VC_PROCESSING_UNIT      0x05
#define AMI_UVC_VC_EXTENSION_UNIT       0x06
#define AMI_UVC_VC_ENCODING_UNIT        0x07

// Video Class-Specific VS Interface Descriptor Subtypes
#define AMI_UVC_VS_ UNDEFINED           0x00
#define AMI_UVC_VS_INPUT_HEADER         0x01
#define AMI_UVC_VS_OUTPUT_HEADER        0x02
#define AMI_UVC_VS_STILL_IMAGE_FRAME    0x03
#define AMI_UVC_VS_FORMAT_UNCOMPRESSED  0x04
#define AMI_UVC_VS_FRAME_UNCOMPRESSED   0x05
#define AMI_UVC_VS_FORMAT_MJPEG         0x06
#define AMI_UVC_VS_FRAME_MJPEG          0x07
#define AMI_UVC_VS_Reserved_08          0x08
#define AMI_UVC_VS_Reserved_09          0x09
#define AMI_UVC_VS_FORMAT_MPEG2TS       0x0A
#define AMI_UVC_Reserved                0x0B
#define AMI_UVC_VS_FORMAT_DV            0x0C
#define AMI_UVC_VS_COLORFORMAT          0x0D
#define AMI_UVC_VS_Reserved_0E          0x0E
#define AMI_UVC_VS_Reserved_0F          0x0F
#define AMI_UVC_VS_FORMAT_FRAME_BASED   0x10
#define AMI_UVC_VS_FRAME_FRAME_BASED    0x11
#define AMI_UVC_VS_FORMAT_STREAM_BASED  0x12
#define AMI_UVC_VS_FORMAT_H264          0x13
#define AMI_UVC_VS_FRAME_H264           0x14
#define AMI_UVC_VS_FORMAT_H264_SIMULCAST    0x15
#define AMI_UVC_VS_FORMAT_VP8           0x16
#define AMI_UVC_VS_FRAME_VP8            0x17
#define AMI_UVC_VS_FORMAT_VP8_SIMULCAST 0x18

// Video Class-Specific Endpoint Descriptor Subtypes
#define AMI_UVC_EP_UNDEFINED    0x00
#define AMI_UVC_EP_GENERAL      0x01
#define AMI_UVC_EP_ENDPOINT     0x02
#define AMI_UVC_EP_INTERRUPT    0x03

// Video Class-Specific Request Codes
#define AMI_UVC_RC_UNDEFINED    0x00
#define AMI_UVC_SET_CUR         0x01
#define AMI_UVC_SET_CUR_ALL     0x11
#define AMI_UVC_GET_CUR         0x81
#define AMI_UVC_GET_MIN         0x82
#define AMI_UVC_GET_MAX         0x83
#define AMI_UVC_GET_RES         0x84
#define AMI_UVC_GET_LEN         0x85
#define AMI_UVC_GET_INFO        0x86
#define AMI_UVC_GET_DEF         0x87
#define AMI_UVC_GET_CUR_ALL     0x91
#define AMI_UVC_GET_MIN_ALL     0x92
#define AMI_UVC_GET_MAX_ALL     0x93
#define AMI_UVC_GET_RES_ALL     0x94
#define AMI_UVC_GET_DEF_ALL     0x97

// Control Selector Codes
#define AMI_UVC_VC _CONTROL_ UNDEFINED      0x00
#define AMI_UVC_VC_VIDEO_POWER_MODE_CONTROL 0x01
#define AMI_UVC_VC_REQUEST_ERROR_CODE_CONTROL   0x02
#define AMI_UVC_VC_Reserved_03              0x03

// Terminal Control Selectors
#define AMI_UVC_TE_CONTROL_UNDEFINED        0x00

// Selector Unit Control Selectors
#define AMI_UVC_SU_CONTROL_UNDEFINED        0x00
#define AMI_UVC_SU_INPUT_SELECT_CONTROL     0x01

// Camera Terminal Control Selectors
#define AMI_UVC_CT_CONTROL_UNDEFINED        0x00
#define AMI_UVC_CT_SCANNING_MODE_CONTROL    0x01
#define AMI_UVC_CT_AE_MODE_CONTROL          0x02
#define AMI_UVC_CT_AE_PRIORITY_CONTROL      0x03
#define AMI_UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL   0x04
#define AMI_UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL   0x05
#define AMI_UVC_CT_FOCUS_ABSOLUTE_CONTROL   0x06
#define AMI_UVC_CT_FOCUS_RELATIVE_CONTROL   0x07
#define AMI_UVC_CT_FOCUS_AUTO_CONTROL       0x08
#define AMI_UVC_CT_IRIS_ABSOLUTE_CONTROL    0x09
#define AMI_UVC_CT_IRIS_RELATIVE_CONTROL    0x0A
#define AMI_UVC_CT_ZOOM_ABSOLUTE_CONTROL    0x0B
#define AMI_UVC_CT_ZOOM_RELATIVE_CONTROL    0x0C
#define AMI_UVC_CT_PANTILT_ABSOLUTE_CONTROL 0x0D
#define AMI_UVC_CT_PANTILT_RELATIVE_CONTROL 0x0E
#define AMI_UVC_CT_ROLL_ABSOLUTE_CONTROL    0x0F
#define AMI_UVC_CT_ROLL_RELATIVE_CONTROL    0x10
#define AMI_UVC_CT_PRIVACY_CONTROL          0x11
#define AMI_UVC_CT_FOCUS_SIMPLE_CONTROL     0x12
#define AMI_UVC_CT_WINDOW_CONTROL           0x13
#define AMI_UVC_CT_REGION_OF_INTEREST_CONTROL   0x14

// Processing Unit Control Selectors
#define AMI_UVC_PU_CONTROL_UNDEFINED                0x00
#define AMI_UVC_PU_BACKLIGHT_COMPENSATION_CONTROL   0x01
#define AMI_UVC_PU_BRIGHTNESS_CONTROL               0x02
#define AMI_UVC_PU_CONTRAST_CONTROL                 0x03
#define AMI_UVC_PU_GAIN_CONTROL                     0x04
#define AMI_UVC_PU_POWER_LINE_FREQUENCY_CONTROL     0x05
#define AMI_UVC_PU_HUE_CONTROL                      0x06
#define AMI_UVC_PU_SATURATION_CONTROL               0x07
#define AMI_UVC_PU_SHARPNESS_CONTROL                0x08
#define AMI_UVC_PU_GAMMA_CONTROL                    0x09
#define AMI_UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL    0x0A
#define AMI_UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL   0x0B
#define AMI_UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL  0x0C
#define AMI_UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL 0x0D
#define AMI_UVC_PU_DIGITAL_MULTIPLIER_CONTROL       0x0E
#define AMI_UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL 0x0F
#define AMI_UVC_PU_HUE_AUTO_CONTROL                 0x10
#define AMI_UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL    0x11
#define AMI_UVC_PU_ANALOG_LOCK_STATUS_CONTROL       0x12
#define AMI_UVC_PU_CONTRAST_AUTO_CONTROL            0x13

// Encoding Unit Control Selectors
#define AMI_UVC_EU_CONTROL_UNDEFINED            0x00
#define AMI_UVC_EU_SELECT_LAYER_CONTROL         0x01
#define AMI_UVC_EU_PROFILE_TOOLSET_CONTROL      0x02
#define AMI_UVC_EU_VIDEO_RESOLUTION_CONTROL     0x03
#define AMI_UVC_EU_ MIN_FRAME_INTERVAL_CONTROL  0x04
#define AMI_UVC_EU_SLICE_MODE_CONTROL           0x05
#define AMI_UVC_EU_RATE_CONTROL_MODE_CONTROL    0x06
#define AMI_UVC_EU_AVERAGE_BITRATE_CONTROL      0x07
#define AMI_UVC_EU_CPB_SIZE_CONTROL             0x08
#define AMI_UVC_EU_PEAK_BIT_RATE_CONTROL        0x09
#define AMI_UVC_EU_QUANTIZATION_PARAMS_CONTROL  0x0A
#define AMI_UVC_EU_SYNC_REF_FRAME_CONTROL       0x0B
#define AMI_UVC_EU_LTR_BUFFER_ CONTROL          0x0C
#define AMI_UVC_EU_LTR_PICTURE_CONTROL          0x0D
#define AMI_UVC_EU_LTR_VALIDATION_CONTROL       0x0E
#define AMI_UVC_EU_LEVEL_IDC_LIMIT_CONTROL      0x0F
#define AMI_UVC_EU_SEI_PAYLOADTYPE_CONTROL      0x10
#define AMI_UVC_EU_QP_RANGE_CONTROL             0x11
#define AMI_UVC_EU_PRIORITY_CONTROL             0x12
#define AMI_UVC_EU_START_OR_STOP_LAYER_CONTROL  0x13
#define AMI_UVC_EU_ERROR_RESILIENCY_CONTROL     0x14

// Extension Unit Control Selectors
#define AMI_UVC_XU_CONTROL_UNDEFINED            0x00

// VideoStreaming Interface Control Selectors
#define AMI_UVC_VS_CONTROL_UNDEFINED            0x00
#define AMI_UVC_VS_PROBE_CONTROL                0x01
#define AMI_UVC_VS_COMMIT_CONTROL               0x02
#define AMI_UVC_VS_STILL_PROBE_CONTROL          0x03
#define AMI_UVC_VS_STILL_COMMIT_CONTROL         0x04
#define AMI_UVC_VS_STILL_IMAGE_TRIGGER_CONTROL  0x05
#define AMI_UVC_VS_STREAM_ERROR_CODE_CONTROL    0x06
#define AMI_UVC_VS_GENERATE_KEY_FRAME_CONTROL   0x07
#define AMI_UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL 0x08
#define AMI_UVC_VS_SYNCH_DELAY_CONTROL          0x09

// USB Terminal Types

// The two following Terminal types describe Terminals that handle signals carried
// over the USB, through isochronous or bulk pipes. These Terminal types are valid
// for both Input and Output Terminals.
#define AMI_UVC_TT_VENDOR_SPECIFIC  0x0100
#define AMI_UVC_TT_STREAMING        0x0101

// Input Terminal Types
// These Terminal Types describe Terminals that are designed to capture video. They
// either are physically part of the video function or can be assumed to be connected
// to it in normal operation. These Terminal Types are valid only for Input Terminals.
#define AMI_UVC_ITT_ VENDOR_SPECIFIC        0x0200
#define AMI_UVC_ITT_CAMERA                  0x0201
#define AMI_UVC_ITT_MEDIA_TRANSPORT_INPUT   0x0202

// Output Terminal Types
// These Terminal types describe Terminals that are designed to render video. They are
// either physically part of the video function or can be assumed to be connected to it
// in normal operation. These Terminal types are only valid for Output Terminals.
#define AMI_UVC_OTT_ VENDOR_SPECIFIC        0x0300
#define AMI_UVC_OTT_DISPLAY                 0x0301
#define AMI_UVC_OTT_MEDIA_TRANSPORT_OUTPUT  0x0302

// External Terminal Types
// These Terminal types describe external resources and connections that do not fit
// under the categories of Input or Output Terminals because they do not necessarily
// translate video signals to or from the user of the computer. Most of them may be
// either Input or Output Terminals.
#define AMI_UVC_TT_EXTERNAL_ VENDOR_SPECIFIC    0x0400
#define AMI_UVC_TT_COMPOSITE_CONNECTOR          0x0401
#define AMI_UVC_TT_SVIDEO_CONNECTOR             0x0402
#define AMI_UVC_TT_COMPONENT_CONNECTOR          0x0403

#define AMI_USB_VIDEO_PROTOCOL_GUID \
  { 0x82820f9f, 0x4197, 0x46d0, { 0xa9, 0x6c, 0xb1, 0xd1, 0x5b, 0xad, 0x98, 0x8e } }

//--------- VIDEO CONTROL DEFINITIONS -----------

#pragma pack (1)

// Video Control interface header
typedef struct _AMI_UVC_CS_INTERFACE_COMMON {
    UINT8   Length;
    UINT8   DescriptorType;
    UINT8   DescriptorSubtype;
    UINT8   UnitId;
} AMI_UVC_CS_INTERFACE_COMMON;

typedef struct _AMI_USB_VC_HEADER {
    UINT8   Length;
    UINT8   DescriptorType;
    UINT8   DescriptorSubtype;
    UINT16  VDCSpecification;
    UINT16  TotalLength;
    UINT32  ClockFrequency;
    UINT8   InCollection;
    UINT8   InterfaceNumber[1]; // [InCollection]
} AMI_USB_VC_HEADER;

// Input terminal definitions
typedef struct _AMI_UVC_INPUT_TERMINAL {
    AMI_UVC_CS_INTERFACE_COMMON Header;
    UINT16  TerminalType;
    UINT8   AssocTerminal;
    UINT8   StringId;
    UINT16  ObjectiveFocalLengthMin;
    UINT16  ObjectiveFocalLengthMax;
    UINT16  OcularFocalLength;
    UINT8   ControlSize;
    UINT8   Control[1]; // [ControlSize]
} AMI_UVC_INPUT_TERMINAL;

// Output terminal definitions
typedef struct _AMI_UVC_STREAMING_TERMINAL {
    AMI_UVC_CS_INTERFACE_COMMON Header;
    UINT16  TerminalType;
    UINT8   AccocTerminal;
    UINT8   SourceId;
    UINT8   StringIndx;
} AMI_UVC_STREAMING_TERMINAL;

// Selector unit definitions
typedef struct _AMI_UVC_SELECTOR_UNIT {
    AMI_UVC_CS_INTERFACE_COMMON Header;
    UINT8   NumInputPins;
    UINT8   SourceId[1];    // [NumInputPins]
//    UINT8   StringIndx;
} AMI_UVC_SELECTOR_UNIT;

// Processing unit definitions
typedef struct _AMI_UVC_PROCESSING_UNIT {
    AMI_UVC_CS_INTERFACE_COMMON Header;
    UINT8   SourceId;
    UINT16  MaxMultiplier;  // Max digital magnification, times 100
    UINT8   ControlSize;
    UINT8   Control[1];     // [ControlSize]
//    UINT8   StringIndx;
//    UINT8   VideoStandards; // PAL/SECAM/NTSC
} AMI_UVC_PROCESSING_UNIT;

// Encoding unit definitions
typedef struct _AMI_UVC_ENCODING_UNIT {
    AMI_UVC_CS_INTERFACE_COMMON Header;
    UINT8   SourceId;
    UINT8   StringIndx;
    UINT8   ControlSize;        // Always 3
    UINT8   Control[3];         // [ControlSize]
    UINT8   ControlRuntime[3];  // [ControlSize]
} AMI_UVC_ENCODING_UNIT;

// Extension unit definitions
typedef struct _AMI_UVC_EXTENSION_UNIT {
    AMI_UVC_CS_INTERFACE_COMMON Header;
    EFI_GUID    VendorEuGuid;
    UINT8   NumControls;
    UINT8   NumInputPins;
    UINT8   SourceId[1];    // [NumInputPins]
//    UINT8   ControlSize;
//    UINT8   Control[ControlSize];
//    UINT8   StringIndx;
} AMI_UVC_EXTENSION_UNIT;

//------- END OF VIDEO CONTROL DEFINITIONS ---------


//--------- VIDEO STREAM DEFINITIONS -----------

// Video Stream interface header
typedef struct _AMI_USB_VS_HEADER_DESC {
    UINT8   Length;
    UINT8   DescriptorType;
    UINT8   DescriptorSubtype;
    UINT8   NumFormats;
    UINT16  TotalLength;
    UINT8   EndpointAddress;
    UINT8   Info;
    UINT8   TerminalLink;
    UINT8   StillCaptureMethod;
    UINT8   TriggerSupport;
    UINT8   TriggerUsage;
    UINT8   ControlSize;
    UINT8   Control[1];   // [ControlSize];
} AMI_USB_VS_HEADER_DESC;

typedef struct _AMI_USB_VS_FRAME_UNCOMPRESSED {
    UINT8   Length;
    UINT8   DescriptorType;
    UINT8   DescriptorSubtype;
    UINT8   FrameDescIndex;
    UINT8   StillImageSupported;
    UINT16  Width;
    UINT16  Hight;
    UINT32  MinBItRate;
    UINT32  MaxBitRate;
    UINT32  MaxVideoFrameBufferSize;
    UINT32  DefaultFrameInterval;
    UINT8   FrameIntervalCount;
    UINT8   FrameIntervalType;
} AMI_USB_VS_FRAME_UNCOMPRESSED;

//------- END OF VIDEO STREAM DEFINITIONS ---------

typedef struct _AMI_USB_VS_STREAM_HEADER {
    UINT8   HeaderLength;
    struct {
        UINT8   Fid : 1;    // Frame Identifier, toggles at each frame start
        UINT8   Eof : 1;    // End of frame
        UINT8   PtsPresent  : 1;    // Presentation time stamp presence indicator
        UINT8   ScrPresent  : 1;    // Source clock reference presence indicator
        UINT8   Reserved    : 1;
        UINT8   IsStillImg  : 1;
        UINT8   Err : 1;
        UINT8   Eoh : 1;    // End Of Header
    } BitField;
    UINT8   Pts[4]; // Presentation time stamp
    UINT8   Scr[6]; // Source clock reference
} AMI_USB_VS_STREAM_HEADER;


//------- UNCOMPRESSED PAYLOAD DEFINITIONS --------
#define AMI_UVC_YUY2_GUID \
    { 0x32595559, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } }

#define AMI_UVC_NV12_GUID \
    { 0x3231564e, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } }

#define AMI_UVC_M420_GUID \
    { 0x3032344d, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } }

#define AMI_UVC_I420_GUID \
    { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } }

typedef struct _AMI_USB_VS_FORMAT_UNCOMPRESSED {
    UINT8   Length;
    UINT8   DescriptorType;
    UINT8   DescriptorSubtype;
    UINT8   FormatIndex;
    UINT8   NumFrameDescriptors;
    EFI_GUID    FormatGuid;
    UINT8   BitsPerPixel;
    UINT8   DefaultFrameIndex;
    UINT8   AspectRatioX;
    UINT8   AspectRatioY;
    UINT8   InterfaceFlags;
    UINT8   CopyProtect;
} AMI_USB_VS_FORMAT_UNCOMPRESSED;

//------- END OF UNCOMPRESSED PAYLOAD DEFINITIONS --------

typedef struct _AMI_UVC_CONTROL {
    BOOLEAN Supported;
    UINT16  Min;
    UINT16  Max;
    UINT16  Cur;
    UINT16  Res;
} AMI_UVC_CONTROL;

typedef struct {
    UINT16  Hint;
    UINT8   FormatIndex;
    UINT8   FrameIndex;
    UINT32  FrameInterval;
    UINT16  KeyFrameRate;
    UINT16  PFrameRate;
    UINT16  CompQuality;
    UINT16  CompWindowSize;
    UINT16  Delay;
    UINT32  MaxVideoFrameSize;
    UINT32  MaxPayloadTransferSize;   // probe returns 26 bytes, this is the last field
    UINT32  ClockFrequency;
    UINT8   FramingInfo;
    UINT8   PreferedVersion;
    UINT8   MinVersion;
    UINT8   MaxVersion;    // end of UVC1.1 structure; the below fields are in UVC1.5
    UINT8   Usage;
    UINT8   BitDepthLuma;
    UINT8   Settings;
    UINT8   MaxNumberOfRefFramesPlus1;
    UINT16  RateControlModes;
    UINT16  LayoutPerStream[4];
} AMI_UVC_PROBE_COMMIT_DATA;

// Video Control properties
typedef enum {
    EfiUvcScanningMode          = 0x00,
    EfiUvcAutoExposureMode      = 0x01,
    EfiUvcAutoExposurePriority  = 0x02,
    EfiUvcExposureTimeAbsolute  = 0x03,
    EfiUvcExposureTimeRelative  = 0x04,
    EfiUvcFocusAbsolute         = 0x05,
    EfiUvcFocusRelative         = 0x06,
    EfiUvcFocusSimpleRange      = 0x07,
    EfiUvcFocusAuto             = 0x08,
    EfiUvcIrisAbsolute          = 0x09,
    EfiUvcIrisRelative          = 0x0a,
    EfiUvcZoomAbsolute          = 0x0b,
    EfiUvcZoomRelative          = 0x0c,
    EfiUvcPanTiltAbsolute       = 0x0d,
    EfiUvcPanTiltRelative       = 0x0e,
    EfiUvcRollAbsolute          = 0x0f,
    EfiUvcRollRelative          = 0x10,
    EfiUvcPrivacy               = 0x11,
    EfiUvcDigitalWindow         = 0x12,
    EfiUvcRoi                   = 0x13,
    EfiUvcBacklightCompensation = 0x14,
    EfiUvcBrightness            = 0x15,
    EfiUvcContrast              = 0x16,
    EfiUvcContrastAuto          = 0x17,
    EfiUvcGain                  = 0x18,
    EfiUvcPowerLineFrequency    = 0x19,
    EfiUvcHue                   = 0x1a,
    EfiUvcHueAuto               = 0x1b,
    EfiUvcSaturation            = 0x1c,
    EfiUvcSharpness             = 0x1d,
    EfiUvcGamma                 = 0x1e,
    EfiUvcWbTemp                = 0x1f,
    EfiUvcWbTempAuto            = 0x20,
    EfiUvcWbComponent           = 0x21,
    EfiUvcWbComponentAuto       = 0x22,
    EfiUvcDigitalMultiplier     = 0x23,
    EfiUvcDigitalMultiplierLimit= 0x24,
    EfiUvcAnalogVideoStandard   = 0x25,
    EfiUvcAnalogVideoLock       = 0x26,
    EfiUvcPropertyMaxIndex      = 0x27,
} AMI_UVC_PROPERTY_ID;

typedef enum {
    EfiUvcPropertyCheckPresence,
    EfiUvcPropertySetCur,
    EfiUvcPropertyGetCur,
    EfiUvcPropertyGetMin,
    EfiUvcPropertyGetMax,
    EfiUvcPropertyGetRes,
    EfiUvcPropertyGetDef,
    EfiUvcPropertyGetInfo,
    EfiUvcPropertyGetLen,
    EfiUvcPropertyReqMax
} AMI_UVC_PROPERTY_REQUEST;

typedef struct _AMI_USB_VIDEO_PROTOCOL AMI_USB_VIDEO_PROTOCOL;

typedef enum {
    UvcVsInfoDeviceName,
    UvcVsInfoResListSize,
    UvcVsInfoResolution,
    UvcVsInfoFrameRate,
    UvcVsInfoCurrentResolution,
    UvcVsInfoDefaultResolution,
    UvcVsInfoMax
} AMI_UVC_VS_INFO_TYPE;

typedef struct {
    UINT16  X;
    UINT16  Y;
} AMI_UVC_V_BOUNDS;

typedef struct {
    UINT8   FrameDescriptorIndex;
    AMI_UVC_V_BOUNDS Res;
} AMI_UVC_VS_VRESOLUTION;

#pragma pack()

typedef EFI_STATUS (EFIAPI *AMI_USB_VIDEO_PROTOCOL_GET_PARAMS) (
  IN AMI_USB_VIDEO_PROTOCOL     *This,
  IN AMI_UVC_VS_INFO_TYPE       Type,  
  OUT VOID                      *DevInfo
);

typedef EFI_STATUS (EFIAPI *AMI_USB_VIDEO_PROTOCOL_SET_PARAMS) (
  IN AMI_USB_VIDEO_PROTOCOL     *This,
  IN AMI_UVC_VS_INFO_TYPE       Type,
  IN OUT VOID                   *Params
);

typedef VOID (EFIAPI *AMI_USB_VIDEO_PROCESS_FRAME) (
  IN  AMI_USB_VIDEO_PROTOCOL    *This,
  IN VOID                       *VideoFrame
);

typedef EFI_STATUS (EFIAPI *AMI_USB_VIDEO_PROTOCOL_START_VIDEO) (
  IN AMI_USB_VIDEO_PROTOCOL         *This,
  IN AMI_USB_VIDEO_PROCESS_FRAME    ProcessFrame
);

typedef EFI_STATUS (EFIAPI *AMI_USB_VIDEO_PROTOCOL_STOP_VIDEO) (
  IN  AMI_USB_VIDEO_PROTOCOL    *This
);

typedef EFI_STATUS (EFIAPI *AMI_USB_VIDEO_PROTOCOL_PROPERTY) (
  IN AMI_USB_VIDEO_PROTOCOL     *This,
  IN AMI_UVC_PROPERTY_REQUEST   Req,
  IN AMI_UVC_PROPERTY_ID        Id,
  IN OUT VOID                   *Data
);

typedef struct _AMI_USB_VIDEO_PROTOCOL {
  AMI_USB_VIDEO_PROTOCOL_GET_PARAMS     GetParameters;
  AMI_USB_VIDEO_PROTOCOL_SET_PARAMS     SetParameters;
  AMI_USB_VIDEO_PROTOCOL_START_VIDEO    StartVideo;
  AMI_USB_VIDEO_PROTOCOL_STOP_VIDEO     StopVideo;
  AMI_USB_VIDEO_PROTOCOL_PROPERTY       Property;
  EFI_EVENT                             DisconnectEvent;
} AMI_USB_VIDEO_PROTOCOL;

extern EFI_GUID gAmiUsbVideoProtocolGuid;

#endif // __AMI_USB_VIDEO_PROTOCOL__H__

//**********************************************************************
//**********************************************************************
//**                                                                  **
//**        (C)Copyright 1985-2015, American Megatrends, Inc.         **
//**                                                                  **
//**                       All Rights Reserved.                       **
//**                                                                  **
//**      5555 Oakbrook Parkway, Suite 200, Norcross, GA 30093        **
//**                                                                  **
//**                       Phone: (770)-246-8600                      **
//**                                                                  **
//**********************************************************************
//**********************************************************************
