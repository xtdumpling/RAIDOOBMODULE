

typedef struct _ME_BIOS_SPS_NVS_REGION { 
  UINT32      THNU, PURA, PURB, PURE, PRBM;
  UINT8       HECI1Visibility;               //830
  UINT8       HECI2Visibility;               //831
  UINT8	      WESKU;			     // 832
  UINT8       OSLoad;			     // 833
  UINT8	      NodeManager;                  //800
  UINT8       StatesConfig;                 //801_PTEN, CSEN, CXOS:2, C6EN, C7EN, MWOS, HWAL, 
  UINT8	      Padding[2];
  UINT32      PETE;			// 837
  UINT32      PST0;			// 841
  UINT32      PST1;			// 845
  UINT32      PST2;			// 837
  UINT32      PST3;			// 841
  UINT32      TST0;			// 837
  UINT32      TST1;			// 841
  UINT32      TST2;			// 845
  UINT32      TST3;			// 845
} ME_BIOS_SPS_NVS_REGION;

typedef struct _me_nvs_variables {
    EFI_PHYSICAL_ADDRESS          Addr;
	ME_BIOS_SPS_NVS_REGION 		  *Area;
} SPS_ME_ACPIN_VS_REGION;