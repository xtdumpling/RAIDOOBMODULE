
1. BIOS config flow chart.

DownloadBiosCfgFromBMC
  |          |
  |          +-- Download data from BMC and save it to 'INBAND_REGION'
  V
SmcInBandCallback
  |          |
  |          +-- InBandUpdateBiosCfg - Read 'INBAND_REGION' and check update bit in header.
  |          |                         If it's true, update to variables.
  |          |
  |          +-- CombineBiosCfg - Compare current variables combined data and 'INBAND_REGION' data.
  |                               If data is changed, update current variables combined data to 'INBAND_REGION'.
  |
  V
=============== Boot to OS
  |
  V
SmcInBandSwSmiE6Handler - Sum triggers E6 SMI and BIOS will enter the handler.
                          BIOS will get data from sum and save it to 'INBAND_REGION'.

