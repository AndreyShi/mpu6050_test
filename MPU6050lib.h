#ifdef __cplusplus
extern "C" {
#endif

    float get_yarn(void);
    float get_pitch(void);
    float get_roll(void);

    float get_AccX(void);
    float get_AccY(void);
    float get_AccZ(void);

    float get_GyroX(void);
    float get_GyroY(void);
    float get_GyroZ(void);

    int init_MPU(void);

#ifdef __cplusplus
}
#endif
