#ifndef BSPS_H
#define BSPS_H

#define CODEC_REG1_VALUE  				0x0303
#define CODEC_REG2_VALUE  				0x0504
#define CODEC_REG3_VALUE  				0x0719
#define CODEC_REG4_VALUE  				0x0904

#define CODEC_DEMUTE_VALUE  			0x0904
#define CODEC_MUTE_VALUE				0x09FF
  
void int_bsps(void);
void ActivateBsp0(int mask);
void DeactiveBsp0(void);
void bsp0_rx_handler(void);
void bsp0_tx_handler(void);
void ConfigCodec(void);
void UpdateCodec(unsigned int CodecVal);

void enable_autobaud(void);

#endif
