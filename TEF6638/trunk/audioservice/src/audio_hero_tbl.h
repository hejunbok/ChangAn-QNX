#ifndef AUDIO_HERO_TBL_H
#define AUDIO_HERO_TBL_H

//////////////////////////////////////////////////////////////////////////////
//ctl_name, addr, addr_cnt
#define AUDIO_SCALER_CTL_TBL \
  OPER1(AUDIO_PRIM_VOL,          AUDIO_Y_Vol_Main1P        OPER2        \
                                 AUDIO_Y_Vol_Main2P,              2  ) \
  OPER1(AUDIO_PRIM_DOWN_SCALER,  AUDIO_Y_Vol_DesScalBMTP,         1  ) \
  OPER1(AUDIO_PRIM_BALANCE_L,    AUDIO_Y_Vol_BalPL,               1  ) \
  OPER1(AUDIO_PRIM_BALANCE_R,    AUDIO_Y_Vol_BalPR,               1  ) \
  OPER1(AUDIO_PRIM_FADER_F,      AUDIO_Y_Vol_FadF,                1  ) \
  OPER1(AUDIO_PRIM_FADER_R,      AUDIO_Y_Vol_FadR,                1  ) \
  OPER1(AUDIO_LOUD_B_STATE_LVL,  AUDIO_Y_Loudf_StatLev,           1  ) \
  OPER1(AUDIO_LOUD_T_STATE_LVL,  AUDIO_Y_Loudf_StatLevT,          1  ) \
  OPER1(AUDIO_LOUD_B_BOOST_MAX,  AUDIO_Y_Loudf_MaxBstB,           1  ) \
  OPER1(AUDIO_LOUD_T_BOOST_MAX,  AUDIO_Y_Loudf_MaxBstT,           1  ) \
  OPER1(AUDIO_P_F_CHNL_GAIN_L,   AUDIO_Y_Vol_ChanGainPFL,         1  ) \
  OPER1(AUDIO_P_F_CHNL_GAIN_R,   AUDIO_Y_Vol_ChanGainPFR,         1  ) \
  OPER1(AUDIO_P_R_CHNL_GAIN_L,   AUDIO_Y_Vol_ChanGainPRL,         1  ) \
  OPER1(AUDIO_P_R_CHNL_GAIN_R,   AUDIO_Y_Vol_ChanGainPRR,         1  ) \
  OPER1(AUDIO_P_F_UP_SCALER,     AUDIO_Y_Vol_UpScalF,             1  ) \
  OPER1(AUDIO_P_R_UP_SCALER,     AUDIO_Y_Vol_UpScalR,             1  ) \
  OPER1(AUDIO_P_F_PRE_SCALER,    AUDIO_Y_Vol_ScalF,               1  ) \
  OPER1(AUDIO_P_R_PRE_SCALER,    AUDIO_Y_Vol_ScalR,               1  ) \
  OPER1(AUDIO_PRIM_SRC_SCALER,   AUDIO_Y_Vol_SrcScalP,            1  ) \
\
  OPER1(AUDIO_NAV_SRC_SCALER,    AUDIO_Y_Vol_SrcScalN,            1  ) \
  OPER1(AUDIO_NAV_VOL,           AUDIO_Y_Vol_Nav,                 1  ) \
  OPER1(AUDIO_NAV_FADE_F,        AUDIO_Y_Sup_NonFL         OPER2       \
                                 AUDIO_Y_Sup_NonFR,               2  ) \
  OPER1(AUDIO_NAV_FADE_R,        AUDIO_Y_Sup_NonRL         OPER2       \
                                 AUDIO_Y_Sup_NonRR,               2  ) \
  OPER1(AUDIO_NAV_CHNL_GAIN,     AUDIO_Y_Vol_ChanGainN,           1  ) \
\
  OPER1(AUDIO_PHONE_SRC_SCALER,  AUDIO_Y_Vol_SrcScalT,            1  ) \
  OPER1(AUDIO_PHONE_VOL,         AUDIO_Y_Vol_Phon,                1  ) \
  OPER1(AUDIO_PHONE_FADE_F,      AUDIO_Y_Sup_TonFL         OPER2       \
                                 AUDIO_Y_Sup_TonFR,               2  ) \
  OPER1(AUDIO_PHONE_FADE_R,      AUDIO_Y_Sup_TonRL         OPER2       \
                                 AUDIO_Y_Sup_TonRR,               2  ) \
  OPER1(AUDIO_PHONE_CHNL_GAIN,   AUDIO_Y_Vol_ChanGainT,           1  ) \
\
  OPER1(AUDIO_N_ON_FRONT_L_MIX,  AUDIO_Y_Sup_NonFL,               1  ) \
  OPER1(AUDIO_N_ON_FRONT_R_MIX,  AUDIO_Y_Sup_NonFR,               1  ) \
  OPER1(AUDIO_N_ON_REAR_L_MIX,   AUDIO_Y_Sup_NonRL,               1  ) \
  OPER1(AUDIO_N_ON_REAR_R_MIX,   AUDIO_Y_Sup_NonRR,               1  ) \
\
  OPER1(AUDIO_T_ON_FRONT_L_MIX,  AUDIO_Y_Sup_TonFL,               1  ) \
  OPER1(AUDIO_T_ON_FRONT_R_MIX,  AUDIO_Y_Sup_TonFR,               1  ) \
  OPER1(AUDIO_T_ON_REAR_L_MIX,   AUDIO_Y_Sup_TonRL,               1  ) \
  OPER1(AUDIO_T_ON_REAR_R_MIX,   AUDIO_Y_Sup_TonRR,               1  )


#define AUDIO_MUTE_CTL_TBL \
  OPER1(AUDIO_PRIM_MUTE,         AUDIO_Y_Mute_P            OPER2       \
                                 AUDIO_X_Mute_StpAttP      OPER2       \
                                 AUDIO_X_Mute_StpRelP,            3  ) \
  OPER1(AUDIO_NAV_MUTE,          AUDIO_Y_Mute_N            OPER2       \
                                 AUDIO_X_Mute_StpAttN      OPER2       \
                                 AUDIO_X_Mute_StpRelN,            3  ) \
  OPER1(AUDIO_PHONE_MUTE,        AUDIO_Y_Mute_T            OPER2       \
                                 AUDIO_X_Mute_StpAttT      OPER2       \
                                 AUDIO_X_Mute_StpRelT,            3  )

#define AUDIO_FILTER_CTL_TBL \
  OPER1(AUDIO_TONE_BASS,         AUDIO_Y_BMT_a1bHP,               1  ) \
  OPER1(AUDIO_TONE_MID,          AUDIO_Y_BMT_a1mHP,               1  ) \
  OPER1(AUDIO_TONE_TREBLE,       AUDIO_Y_BMT_a1tP,                1  ) \
  OPER1(AUDIO_LOUD_B_FILTER,     AUDIO_Y_Loudf_a1bL,              1  ) \
  OPER1(AUDIO_LOUD_T_FILTER,     AUDIO_Y_Loudf_a1t,               1  ) \
  OPER1(AUDIO_PRIM_DC_FILTER,    AUDIO_Y_DCfilt_a1A,              1  ) \
  OPER1(AUDIO_NAV_FILTER_1,      AUDIO_Y_EqN_b00L,                1  ) \
  OPER1(AUDIO_NAV_FILTER_2,      AUDIO_Y_EqN_b10L,                1  ) \
  OPER1(AUDIO_PHONE_FILTER_1,    AUDIO_Y_EqT_b00L,                1  ) \
  OPER1(AUDIO_PHONE_FILTER_2,    AUDIO_Y_EqT_b10L,                1  ) \
  OPER1(AUDIO_FL_PEQ_FILTER_1,   AUDIO_Y_EqFL_b10L,               1  ) \
  OPER1(AUDIO_FL_PEQ_FILTER_2,   AUDIO_Y_EqFL_b20L,               1  ) \
  OPER1(AUDIO_FL_PEQ_FILTER_3,   AUDIO_Y_EqFL_b30L,               1  ) \
  OPER1(AUDIO_FL_PEQ_FILTER_4,   AUDIO_Y_EqFL_b40L,               1  ) \
  OPER1(AUDIO_FL_PEQ_FILTER_5,   AUDIO_Y_EqFL_b50L,               1  ) \
  OPER1(AUDIO_FL_PEQ_FILTER_6,   AUDIO_Y_EqFL_b60L,               1  ) \
  OPER1(AUDIO_FL_PEQ_FILTER_7,   AUDIO_Y_EqFL_b70L,               1  ) \
  OPER1(AUDIO_FR_PEQ_FILTER_1,   AUDIO_Y_EqFR_b10L,               1  ) \
  OPER1(AUDIO_FR_PEQ_FILTER_2,   AUDIO_Y_EqFR_b20L,               1  ) \
  OPER1(AUDIO_FR_PEQ_FILTER_3,   AUDIO_Y_EqFR_b30L,               1  ) \
  OPER1(AUDIO_FR_PEQ_FILTER_4,   AUDIO_Y_EqFR_b40L,               1  ) \
  OPER1(AUDIO_FR_PEQ_FILTER_5,   AUDIO_Y_EqFR_b50L,               1  ) \
  OPER1(AUDIO_FR_PEQ_FILTER_6,   AUDIO_Y_EqFR_b60L,               1  ) \
  OPER1(AUDIO_FR_PEQ_FILTER_7,   AUDIO_Y_EqFR_b70L,               1  ) \
  OPER1(AUDIO_RL_PEQ_FILTER_1,   AUDIO_Y_EqRL_b10L,               1  ) \
  OPER1(AUDIO_RL_PEQ_FILTER_2,   AUDIO_Y_EqRL_b20L,               1  ) \
  OPER1(AUDIO_RL_PEQ_FILTER_3,   AUDIO_Y_EqRL_b30L,               1  ) \
  OPER1(AUDIO_RL_PEQ_FILTER_4,   AUDIO_Y_EqRL_b40L,               1  ) \
  OPER1(AUDIO_RL_PEQ_FILTER_5,   AUDIO_Y_EqRL_b50L,               1  ) \
  OPER1(AUDIO_RL_PEQ_FILTER_6,   AUDIO_Y_EqRL_b60L,               1  ) \
  OPER1(AUDIO_RL_PEQ_FILTER_7,   AUDIO_Y_EqRL_b70L,               1  ) \
  OPER1(AUDIO_RR_PEQ_FILTER_1,   AUDIO_Y_EqRR_b10L,               1  ) \
  OPER1(AUDIO_RR_PEQ_FILTER_2,   AUDIO_Y_EqRR_b20L,               1  ) \
  OPER1(AUDIO_RR_PEQ_FILTER_3,   AUDIO_Y_EqRR_b30L,               1  ) \
  OPER1(AUDIO_RR_PEQ_FILTER_4,   AUDIO_Y_EqRR_b40L,               1  ) \
  OPER1(AUDIO_RR_PEQ_FILTER_5,   AUDIO_Y_EqRR_b50L,               1  ) \
  OPER1(AUDIO_RR_PEQ_FILTER_6,   AUDIO_Y_EqRR_b60L,               1  ) \
  OPER1(AUDIO_RR_PEQ_FILTER_7,   AUDIO_Y_EqRR_b70L,               1  ) \
  OPER1(AUDIO_GEQ_BAND_1_FILTER, AUDIO_Y_GEq_b10L,                1  ) \
  OPER1(AUDIO_GEQ_BAND_2_FILTER, AUDIO_Y_GEq_b20L,                1  ) \
  OPER1(AUDIO_GEQ_BAND_3_FILTER, AUDIO_Y_GEq_b30L,                1  ) \
  OPER1(AUDIO_GEQ_BAND_4_FILTER, AUDIO_Y_GEq_b40,                 1  ) \
  OPER1(AUDIO_GEQ_BAND_5_FILTER, AUDIO_Y_GEq_b50,                 1  )

#define AUDIO_CTL_TBL \
  AUDIO_SCALER_CTL_TBL \
  AUDIO_MUTE_CTL_TBL \
  AUDIO_FILTER_CTL_TBL

//////////////////////////////////////////////////////////////////////////////
//name, addr
#define AUDIO_MODULE_INLET_TBL \
  OPER1(AUDIO_P_DC_FILTER_INLET,          AUDIO_X_PChanSelPntr         ) \
  OPER1(AUDIO_P_SRC_SCALER_INLET,         AUDIO_X_PProcSelPntr         ) \
  OPER1(AUDIO_P_COMP_EXP_INLET,           AUDIO_X_CompExp_InPntr       ) \
  OPER1(AUDIO_P_GEQ_INLET,                AUDIO_X_GEqInPntr            ) \
  OPER1(AUDIO_P_TONE_INLET,               AUDIO_X_PToneControl_InPntr  ) \
  OPER1(AUDIO_P_MAIN_VOL_INLET,           AUDIO_X_PVol_InPntr          ) \
  OPER1(AUDIO_P_LOUDNESS_INLET,           AUDIO_X_Loudf_InPntr         ) \
  OPER1(AUDIO_P_F_PEQ_INLET,              AUDIO_X_FInPntr              ) \
  OPER1(AUDIO_P_R_PEQ_INLET,              AUDIO_X_RInPntr              ) \
  OPER1(AUDIO_P_SW_L_PEQ_INLET,           AUDIO_X_CenterInPntr         ) \
  OPER1(AUDIO_P_SW_R_PEQ_INLET,           AUDIO_X_SubwInPntr           ) \
  OPER1(AUDIO_P_MAEK_SWC_L_INLET,         AUDIO_X_SwFInPntr            ) \
  OPER1(AUDIO_P_MAKE_SWC_R_INLET,         AUDIO_X_SwRInPntr            ) \
  OPER1(AUDIO_P_F_DELAY_INLET,            AUDIO_X_Delay_F_InPntr       ) \
  OPER1(AUDIO_P_R_DELAY_INLET,            AUDIO_X_Delay_R_InPntr       ) \
  OPER1(AUDIO_P_C_DELAY_INLET,            AUDIO_X_Delay_C_InPntr       ) \
  OPER1(AUDIO_P_SW_DELAY_INLET,           AUDIO_X_Delay_Sw_InPntr      ) \
  OPER1(AUDIO_P_F_POST_PROCESS_INLET,     AUDIO_X_FVolInPntr           ) \
  OPER1(AUDIO_P_R_POST_PROCESS_INLET,     AUDIO_X_RVolInPntr           ) \
  OPER1(AUDIO_P_SW_POST_PROCESS_INLET,    AUDIO_X_SwVolInPntr          ) \
  OPER1(AUDIO_P_F_SUPERPOSITION_INLET,    AUDIO_X_FSupInPntr           ) \
  OPER1(AUDIO_P_R_SUPERPOSITION_INLET,    AUDIO_X_RSupInPntr           ) \
  OPER1(AUDIO_P_SUB_SUPERPOSITION_INLET,  AUDIO_X_SwSupInPntr          ) \
  OPER1(AUDIO_F_SUPERPOSITION_B_INLET,    AUDIO_X_FSupBInPntr          ) \
  OPER1(AUDIO_R_SUPERPOSITION_B_INLET,    AUDIO_X_RSupBInPntr          ) \
  OPER1(AUDIO_SW_SUPERPOSITION_B_INLET,   AUDIO_X_SWSupBInPntr         ) \
  OPER1(AUDIO_SWR_SUPERPOSITION_B_INLET,  AUDIO_X_SWRSupBInPntr        ) \
  OPER1(AUDIO_SUPERPOSITION_B_1_INLET,    AUDIO_X_SupB_InPntr1         ) \
  OPER1(AUDIO_SUPERPOSITION_B_2_INLET,    AUDIO_X_SupB_InPntr2         ) \
  OPER1(AUDIO_SUPERPOSITION_B_3_INLET,    AUDIO_X_SupB_InPntr3         ) \
  OPER1(AUDIO_SUPERPOSITION_B_4_INLET,    AUDIO_X_SupB_InPntr4         ) \
  OPER1(AUDIO_SCD_00_INLET,               AUDIO_X_SCD_InPntr00         ) \
  OPER1(AUDIO_SCD_01_INLET,               AUDIO_X_SCD_InPntr01         ) \
  OPER1(AUDIO_SCD_1_INLET,                AUDIO_X_SCD_InPntr1          ) \
  OPER1(AUDIO_SCD_2_INLET,                AUDIO_X_SCD_InPntr2          ) \
  OPER1(AUDIO_SCD_3_INLET,                AUDIO_X_SCD_InPntr3          ) \
  OPER1(AUDIO_LEVEL_DETECTOR_11_INLET,    AUDIO_X_QPD_InPntr11         ) \
  OPER1(AUDIO_LEVEL_DETECTOR_12_INLET,    AUDIO_X_QPD_InPntr12         ) \
  OPER1(AUDIO_LEVEL_DETECTOR_21_INLET,    AUDIO_X_QPD_InPntr21         ) \
  OPER1(AUDIO_LEVEL_DETECTOR_22_INLET,    AUDIO_X_QPD_InPntr22         ) \
\
  OPER1(AUDIO_S_DC_FILTER_INLET,          AUDIO_X_SChanSelPntr         ) \
  OPER1(AUDIO_S_SRC_SCALER_INLET,         AUDIO_X_SProcSelPntr         ) \
  OPER1(AUDIO_S_POST_PROCESS_INLET,       AUDIO_X_SVolInPntr           ) \
  OPER1(AUDIO_S_SUPERPOSITION_INLET,      AUDIO_X_SSupInPntr           ) \
\
  OPER1(AUDIO_N_SRC_SCALER_INLET,         AUDIO_X_NChanSelPntr         ) \
  OPER1(AUDIO_N_BACK_IN_INLET,            AUDIO_X_NavBackInPntr        ) \
  OPER1(AUDIO_N_SUPERPOSITION_INLET,      AUDIO_X_Sup_NInPntr          ) \
  OPER1(AUDIO_T_SRC_SCALER_IN_INLET,      AUDIO_X_TChanSelPntr         ) \
  OPER1(AUDIO_T_BACK_IN_INLET,            AUDIO_X_PhonBackInPntr       ) \
  OPER1(AUDIO_T_SUPERPOSITION_INLET,      AUDIO_X_Sup_TInPntr          ) \
\
  OPER1(AUDIO_HOST_IIS_OUT0_INLET,        AUDIO_X_HIIS0Outpntr         ) \
  OPER1(AUDIO_HOST_IIS_OUT1_INLET,        AUDIO_X_HIIS1Outpntr         ) \
  OPER1(AUDIO_FRONT_DAC_OUT_INLET,        AUDIO_X_FDACpntr             ) \
  OPER1(AUDIO_REAR_DAC_OUT_INLET,         AUDIO_X_RDACpntr             ) \
\
  OPER1(AUDIO_GPF1_L_INLET,               AUDIO_X_GPF_LInPntr          ) \
  OPER1(AUDIO_GPF1_R_INLET,               AUDIO_X_GPF_RInPntr          ) \
  OPER1(AUDIO_GPF2_INLET,                 AUDIO_X_GPF2_InPntr          ) \
  OPER1(AUDIO_GPF3_L_INLET,               AUDIO_X_GPF3_LInPntr         ) \
  OPER1(AUDIO_GPF3_R_INLET,               AUDIO_X_GPF3_RInPntr         ) \
  OPER1(AUDIO_GPF4_INLET,                 AUDIO_X_GPF4_InPntr          ) \
  OPER1(AUDIO_GPF5_INLET,                 AUDIO_X_GPF5_InPntr          )

#define AUDIO_MODULE_OUTLET_TBL \
  OPER1(AUDIO_P_DC_FILTER_OUTLET,               AUDIO_REL_ADDR(AUDIO_X_DCfilt_OutPL)         ) \
  OPER1(AUDIO_P_SRC_SCALER_OUTLET,              AUDIO_REL_ADDR(AUDIO_X_PChannelMode_OutL)    ) \
  OPER1(AUDIO_P_COMP_EXP_OUTLET,                AUDIO_REL_ADDR(AUDIO_X_CompExp_OutPL)        ) \
  OPER1(AUDIO_P_GEQ_OUTLET,                     AUDIO_REL_ADDR(AUDIO_X_GEq_OutFL)            ) \
  OPER1(AUDIO_P_TONE_OUTLET,                    AUDIO_REL_ADDR(AUDIO_X_ToneOutPL)            ) \
  OPER1(AUDIO_P_MAIN_VOL_OUTLET,                AUDIO_REL_ADDR(AUDIO_X_Vol_OutPL)            ) \
  OPER1(AUDIO_P_LOUDNESS_OUTLET,                AUDIO_REL_ADDR(AUDIO_X_Loudf_OutL)           ) \
  OPER1(AUDIO_P_F_PEQ_OUTLET,                   AUDIO_REL_ADDR(AUDIO_X_Eq_OutFL)             ) \
  OPER1(AUDIO_P_R_PEQ_OUTLET,                   AUDIO_REL_ADDR(AUDIO_X_Eq_OutRL)             ) \
  OPER1(AUDIO_P_SW_L_PEQ_OUTLET,                AUDIO_REL_ADDR(AUDIO_X_Eq_OutSwL)            ) \
  OPER1(AUDIO_P_SW_R_PEQ_OUTLET,                AUDIO_REL_ADDR(AUDIO_X_Eq_OutSwR)            ) \
  OPER1(AUDIO_P_MAKE_SWC_L_OUTLET,              AUDIO_REL_ADDR(AUDIO_X_MkSubwLeft)           ) \
  OPER1(AUDIO_P_MAKE_SWC_R_OUTLET,              AUDIO_REL_ADDR(AUDIO_X_MkSubwRight)          ) \
  OPER1(AUDIO_P_F_DELAY_OUTLET,                 AUDIO_REL_ADDR(AUDIO_X_Delay_FL_Out)         ) \
  OPER1(AUDIO_P_R_DELAY_OUTLET,                 AUDIO_REL_ADDR(AUDIO_X_Delay_RL_Out)         ) \
  OPER1(AUDIO_P_C_DELAY_OUTLET,                 AUDIO_REL_ADDR(AUDIO_X_Delay_SwL_Out)        ) \
  OPER1(AUDIO_P_SW_R_DELAY_OUTLET,              AUDIO_REL_ADDR(AUDIO_X_Delay_SwR_Out)        ) \
  OPER1(AUDIO_P_F_POST_PROCESS_OUTLET,          AUDIO_REL_ADDR(AUDIO_X_Vol_OutFL)            ) \
  OPER1(AUDIO_P_R_POST_PROCESS_OUTLET,          AUDIO_REL_ADDR(AUDIO_X_Vol_OutRL)            ) \
  OPER1(AUDIO_P_SW_POST_PROCESS_OUTLET,         AUDIO_REL_ADDR(AUDIO_X_Vol_OutSwL)           ) \
  OPER1(AUDIO_P_F_SUPERPOSITION_OUTLET,         AUDIO_REL_ADDR(AUDIO_X_FrontOutL)            ) \
  OPER1(AUDIO_P_R_SUPERPOSITION_OUTLET,         AUDIO_REL_ADDR(AUDIO_X_RearOutL)             ) \
  OPER1(AUDIO_P_SUB_SUPERPOSITION_OUTLET,       AUDIO_REL_ADDR(AUDIO_X_SwOutL)               ) \
  OPER1(AUDIO_F_SUPERPOSITION_B_OUTLET,         AUDIO_REL_ADDR(AUDIO_X_SupB_OutFL)           ) \
  OPER1(AUDIO_R_SUPERPOSITION_B_OUTLET,         AUDIO_REL_ADDR(AUDIO_X_SupB_OutRL)           ) \
  OPER1(AUDIO_SUB_SUPERPOSITION_B_OUTLET,       AUDIO_REL_ADDR(AUDIO_X_SupB_OutSW)           ) \
  OPER1(AUDIO_CHIME_GENERATOR_OUTLET,           AUDIO_REL_ADDR(AUDIO_X_Chime_Out)            ) \
  OPER1(AUDIO_PDC_GENERATOR_OUTLET,             AUDIO_REL_ADDR(AUDIO_X_PDC2_Out)             ) \
  OPER1(AUDIO_CLICK_CLACK_GENERATOR_OUTLET,     AUDIO_REL_ADDR(AUDIO_X_Chime_Cl_Out)         ) \
  OPER1(AUDIO_LIMITER_FR_OUTLET,                AUDIO_REL_ADDR(AUDIO_X_FCLim_FlgGenFlagFR)   ) \
  OPER1(AUDIO_LIMITER_SW_L_OUTLET,              AUDIO_REL_ADDR(AUDIO_X_FCLim_FlgGenFlagSwL)  ) \
  OPER1(AUDIO_LIMITER_SW_R_OUTLET,              AUDIO_REL_ADDR(AUDIO_X_FCLim_FlgGenFlagSwR)  ) \
  OPER1(AUDIO_LIMITER_RS_OUTLET,                AUDIO_REL_ADDR(AUDIO_X_FCLim_FlgGenFlagRS)   ) \
  OPER1(AUDIO_LEVEL_DETECTOR1_OUTLET,           AUDIO_REL_ADDR(AUDIO_X_QPD_Peak1)            ) \
  OPER1(AUDIO_LEVEL_DETECTOR1_NON_AVER_OUTLET,  AUDIO_REL_ADDR(AUDIO_X_QPD_NonAverPeak1)     ) \
  OPER1(AUDIO_LEVEL_DETECTOR2_OUTLET,           AUDIO_REL_ADDR(AUDIO_X_QPD_Peak2)            ) \
\
  OPER1(AUDIO_S_DC_FILTER_OUTLET,               AUDIO_REL_ADDR(AUDIO_X_DCfilt_OutSL)         ) \
  OPER1(AUDIO_S_SRC_SCALER_OUTLET,              AUDIO_REL_ADDR(AUDIO_X_Vol_OutScalSL)        ) \
  OPER1(AUDIO_S_POST_PROCESS_OUTLET,            AUDIO_REL_ADDR(AUDIO_X_Vol_OutSL)            ) \
  OPER1(AUDIO_S_SUPERPOSITION_OUTLET,           AUDIO_REL_ADDR(AUDIO_X_SecOutL)              ) \
\
  OPER1(AUDIO_N_BEFORE_EQ_OUTLET,               AUDIO_REL_ADDR(AUDIO_X_Navb4EQ)              ) \
  OPER1(AUDIO_N_OUT_OUTLET,                     AUDIO_REL_ADDR(AUDIO_X_NavOut)               ) \
  OPER1(AUDIO_T_BEFORE_EQ_OUTLET,               AUDIO_REL_ADDR(AUDIO_X_Phonb4EQ)             ) \
  OPER1(AUDIO_T_OUT_OUTLET,                     AUDIO_REL_ADDR(AUDIO_X_PhonOut)              ) \
\
  OPER1(AUDIO_SRC_1_OUTLET,                     AUDIO_REL_ADDR(AUDIO_X_SRC1InL)              ) \
  OPER1(AUDIO_SRC_2_OUTLET,                     AUDIO_REL_ADDR(AUDIO_X_SRC2InL)              ) \
  OPER1(AUDIO_SRC_3_OUTLET,                     AUDIO_REL_ADDR(AUDIO_X_SRC3InL)              ) \
  OPER1(AUDIO_HOST_1_OUTLET,                    AUDIO_REL_ADDR(AUDIO_X_HIIS0InL)             ) \
  OPER1(AUDIO_HOST_2_OUTLET,                    AUDIO_REL_ADDR(AUDIO_X_HIIS1InL)             ) \
  OPER1(AUDIO_AD01_OUTLET,                      AUDIO_REL_ADDR(AUDIO_X_AD01InL)              ) \
  OPER1(AUDIO_AD23_OUTLET,                      AUDIO_REL_ADDR(AUDIO_X_AD23InL)              ) \
  OPER1(AUDIO_SIN_GENERATOR_OUTLET,             AUDIO_REL_ADDR(AUDIO_X_SinGen_OutL)          ) \
  OPER1(AUDIO_NOISE_GENERATOR_OUTLET,           AUDIO_REL_ADDR(AUDIO_X_NG_OutL)              ) \
  OPER1(AUDIO_NULL_OUTLET,                      AUDIO_REL_ADDR(AUDIO_X_ZeroSigL)             ) \
\
  OPER1(AUDIO_GPF1_L_OUTLET,                    AUDIO_REL_ADDR(AUDIO_X_GPF_OutL)             ) \
  OPER1(AUDIO_GPF1_R_OUTLET,                    AUDIO_REL_ADDR(AUDIO_X_GPF_OutR)             ) \
  OPER1(AUDIO_GPF2_OUTLET,                      AUDIO_REL_ADDR(AUDIO_X_GPF2_OutL)            ) \
  OPER1(AUDIO_GPF3_L_OUTLET,                    AUDIO_REL_ADDR(AUDIO_X_GPF3_OutL)            ) \
  OPER1(AUDIO_GPF3_R_OUTLET,                    AUDIO_REL_ADDR(AUDIO_X_GPF3_OutR)            ) \
  OPER1(AUDIO_GPF4_OUTLET,                      AUDIO_REL_ADDR(AUDIO_X_GPF4_OutL)            ) \
  OPER1(AUDIO_GPF5_OUTLET,                      AUDIO_REL_ADDR(AUDIO_X_GPF5_OutL)            )
//////////////////////////////////////////////////////////////////////////////
//name, feature, ctl, inlet, outlet
#define AUDIO_MODULE_TBL \
  OPER1(AUDIO_P_DC_FILTER,            AUDIO_DC_FILTER,         AUDIO_PRIM_DC_FILTER,          AUDIO_P_DC_FILTER_INLET,          AUDIO_P_DC_FILTER_OUTLET          ) \
  OPER1(AUDIO_P_SRC_SCALER,           AUDIO_SOURCE_SCALER,     AUDIO_PRIM_SRC_SCALER,         AUDIO_P_SRC_SCALER_INLET,         AUDIO_P_SRC_SCALER_OUTLET         ) \
  OPER1(AUDIO_P_COMPRESSOR,           AUDIO_COMPRESSOR,        AUDIO_CTL_NULL,                AUDIO_P_COMP_EXP_INLET,           AUDIO_P_COMP_EXP_OUTLET           ) \
  OPER1(AUDIO_P_GEQ,                  AUDIO_GRAPHIC_EQ,        AUDIO_GEQ_BAND_1_FILTER OPER2                                                                        \
                                                               AUDIO_GEQ_BAND_2_FILTER OPER2                                                                        \
                                                               AUDIO_GEQ_BAND_3_FILTER OPER2                                                                        \
                                                               AUDIO_GEQ_BAND_4_FILTER OPER2                                                                        \
                                                               AUDIO_GEQ_BAND_5_FILTER,       AUDIO_P_GEQ_INLET,                AUDIO_P_GEQ_OUTLET                ) \
  OPER1(AUDIO_P_DOWN_SCALER,          AUDIO_DOWN_SCALER,       AUDIO_PRIM_DOWN_SCALER,        AUDIO_P_TONE_INLET,               AUDIO_P_TONE_OUTLET               ) \
  OPER1(AUDIO_P_TONE,                 AUDIO_TONE,              AUDIO_TONE_BASS         OPER2                                                                                  \
                                                               AUDIO_TONE_MID          OPER2                                                                                  \
                                                               AUDIO_TONE_TREBLE,             AUDIO_P_TONE_INLET,               AUDIO_P_TONE_OUTLET               ) \
  OPER1(AUDIO_P_MAIN_VOL,             AUDIO_MAIN_VOL,          AUDIO_PRIM_VOL,                AUDIO_P_MAIN_VOL_INLET,           AUDIO_P_MAIN_VOL_OUTLET           ) \
  OPER1(AUDIO_P_LOUDNESS,             AUDIO_LOUDNESS,          AUDIO_LOUD_B_STATE_LVL  OPER2                                                                                  \
                                                               AUDIO_LOUD_T_STATE_LVL  OPER2                                                                                  \
                                                               AUDIO_LOUD_B_BOOST_MAX  OPER2                                                                                  \
                                                               AUDIO_LOUD_T_BOOST_MAX  OPER2                                                                                  \
                                                               AUDIO_LOUD_B_FILTER     OPER2                                                                                  \
                                                               AUDIO_LOUD_T_FILTER,           AUDIO_P_LOUDNESS_INLET,           AUDIO_P_LOUDNESS_OUTLET           ) \
  OPER1(AUDIO_P_FL_PEQ,               AUDIO_LEFT_PARM_EQ,      AUDIO_FL_PEQ_FILTER_1   OPER2                                                                                  \
                                                               AUDIO_FL_PEQ_FILTER_2   OPER2                                                                                  \
                                                               AUDIO_FL_PEQ_FILTER_3   OPER2                                                                                  \
                                                               AUDIO_FL_PEQ_FILTER_4   OPER2                                                                                  \
                                                               AUDIO_FL_PEQ_FILTER_5   OPER2                                                                                  \
                                                               AUDIO_FL_PEQ_FILTER_6   OPER2                                                                                  \
                                                               AUDIO_FL_PEQ_FILTER_7,         AUDIO_P_F_PEQ_INLET,              AUDIO_P_F_PEQ_OUTLET              ) \
  OPER1(AUDIO_P_FR_PEQ,               AUDIO_RIGHT_PARM_EQ,     AUDIO_FR_PEQ_FILTER_1   OPER2                                                                                  \
                                                               AUDIO_FR_PEQ_FILTER_2   OPER2                                                                                  \
                                                               AUDIO_FR_PEQ_FILTER_3   OPER2                                                                                  \
                                                               AUDIO_FR_PEQ_FILTER_4   OPER2                                                                                  \
                                                               AUDIO_FR_PEQ_FILTER_5   OPER2                                                                                  \
                                                               AUDIO_FR_PEQ_FILTER_6   OPER2                                                                                  \
                                                               AUDIO_FR_PEQ_FILTER_7,         AUDIO_P_F_PEQ_INLET,              AUDIO_P_F_PEQ_OUTLET              ) \
  OPER1(AUDIO_P_RL_PEQ,               AUDIO_LEFT_PARM_EQ,      AUDIO_RL_PEQ_FILTER_1   OPER2                                                                                  \
                                                               AUDIO_RL_PEQ_FILTER_2   OPER2                                                                                  \
                                                               AUDIO_RL_PEQ_FILTER_3   OPER2                                                                                  \
                                                               AUDIO_RL_PEQ_FILTER_4   OPER2                                                                                  \
                                                               AUDIO_RL_PEQ_FILTER_5   OPER2                                                                                  \
                                                               AUDIO_RL_PEQ_FILTER_6   OPER2                                                                                  \
                                                               AUDIO_RL_PEQ_FILTER_7,         AUDIO_P_R_PEQ_INLET,              AUDIO_P_R_PEQ_OUTLET              ) \
  OPER1(AUDIO_P_RR_PEQ,               AUDIO_RIGHT_PARM_EQ,     AUDIO_RR_PEQ_FILTER_1   OPER2                                                                                  \
                                                               AUDIO_RR_PEQ_FILTER_2   OPER2                                                                                  \
                                                               AUDIO_RR_PEQ_FILTER_3   OPER2                                                                                  \
                                                               AUDIO_RR_PEQ_FILTER_4   OPER2                                                                                  \
                                                               AUDIO_RR_PEQ_FILTER_5   OPER2                                                                                  \
                                                               AUDIO_RR_PEQ_FILTER_6   OPER2                                                                                  \
                                                               AUDIO_RR_PEQ_FILTER_7,         AUDIO_P_R_PEQ_INLET,              AUDIO_P_R_PEQ_OUTLET              ) \
  OPER1(AUDIO_P_F_SCALER,             AUDIO_PRESCALER,         AUDIO_P_F_PRE_SCALER,          AUDIO_P_F_PEQ_INLET,              AUDIO_P_F_PEQ_OUTLET              ) \
  OPER1(AUDIO_P_R_SCALER,             AUDIO_PRESCALER,         AUDIO_P_R_PRE_SCALER,          AUDIO_P_R_PEQ_INLET,              AUDIO_P_R_PEQ_OUTLET              ) \
  OPER1(AUDIO_P_F_DELAY,              AUDIO_MULTICHNL_DELAY,   AUDIO_CTL_NULL,                AUDIO_P_F_DELAY_INLET,            AUDIO_P_F_DELAY_OUTLET            ) \
  OPER1(AUDIO_P_R_DELAY,              AUDIO_MULTICHNL_DELAY,   AUDIO_CTL_NULL,                AUDIO_P_R_DELAY_INLET,            AUDIO_P_R_DELAY_OUTLET            ) \
  OPER1(AUDIO_P_F_CHNL_GAIN,          AUDIO_CHNL_GAIN,         AUDIO_P_F_CHNL_GAIN_L   OPER2                                                                                  \
                                                               AUDIO_P_F_CHNL_GAIN_R,         AUDIO_P_F_POST_PROCESS_INLET,     AUDIO_P_F_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_R_CHNL_GAIN,          AUDIO_CHNL_GAIN,         AUDIO_P_R_CHNL_GAIN_L   OPER2                                                                                  \
                                                               AUDIO_P_R_CHNL_GAIN_R,         AUDIO_P_R_POST_PROCESS_INLET,     AUDIO_P_R_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_SW_CHNL_GAIN,         AUDIO_CHNL_GAIN,         AUDIO_CTL_NULL,                AUDIO_P_SW_POST_PROCESS_INLET,    AUDIO_P_SW_POST_PROCESS_OUTLET    ) \
  OPER1(AUDIO_P_F_BALANCE,            AUDIO_BALANCE,           AUDIO_PRIM_BALANCE_L    OPER2                                                                                  \
                                                               AUDIO_PRIM_BALANCE_R,          AUDIO_P_F_POST_PROCESS_INLET,     AUDIO_P_F_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_R_BALANCE,            AUDIO_BALANCE,           AUDIO_PRIM_BALANCE_L    OPER2                                                                                  \
                                                               AUDIO_PRIM_BALANCE_R,          AUDIO_P_R_POST_PROCESS_INLET,     AUDIO_P_R_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_SW_BALANCE,           AUDIO_BALANCE,           AUDIO_PRIM_BALANCE_L    OPER2                                                                                  \
                                                               AUDIO_PRIM_BALANCE_R,          AUDIO_P_SW_POST_PROCESS_INLET,    AUDIO_P_SW_POST_PROCESS_OUTLET    ) \
  OPER1(AUDIO_P_F_FADER,              AUDIO_FADE,              AUDIO_PRIM_FADER_F,            AUDIO_P_F_POST_PROCESS_INLET,     AUDIO_P_F_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_R_FADER,              AUDIO_FADE,              AUDIO_PRIM_FADER_R,            AUDIO_P_R_POST_PROCESS_INLET,     AUDIO_P_R_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_SW_FADE,              AUDIO_FADE,              AUDIO_CTL_NULL,                AUDIO_P_SW_POST_PROCESS_INLET,    AUDIO_P_SW_POST_PROCESS_OUTLET    ) \
  OPER1(AUDIO_P_F_UPSCALER,           AUDIO_UP_SCALER,         AUDIO_P_F_UP_SCALER,           AUDIO_P_F_POST_PROCESS_INLET,     AUDIO_P_F_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_R_UPSCALER,           AUDIO_UP_SCALER,         AUDIO_P_R_UP_SCALER,           AUDIO_P_R_POST_PROCESS_INLET,     AUDIO_P_R_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_SW_UPSCALER,          AUDIO_UP_SCALER,         AUDIO_CTL_NULL,                AUDIO_P_SW_POST_PROCESS_INLET,    AUDIO_P_SW_POST_PROCESS_OUTLET    ) \
  OPER1(AUDIO_P_F_MUTE,               AUDIO_MUTE,              AUDIO_PRIM_MUTE,               AUDIO_P_F_POST_PROCESS_INLET,     AUDIO_P_F_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_R_MUTE,               AUDIO_MUTE,              AUDIO_PRIM_MUTE,               AUDIO_P_R_POST_PROCESS_INLET,     AUDIO_P_R_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_SW_MUTE,              AUDIO_MUTE,              AUDIO_PRIM_MUTE,               AUDIO_P_SW_POST_PROCESS_INLET,    AUDIO_P_SW_POST_PROCESS_OUTLET    ) \
  OPER1(AUDIO_P_F_INDEPENDENT_MUTE,   AUDIO_INDEPENDENT_MUTE,  AUDIO_CTL_NULL,                AUDIO_P_F_POST_PROCESS_INLET,     AUDIO_P_F_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_R_INDEPENDENT_MUTE,   AUDIO_INDEPENDENT_MUTE,  AUDIO_CTL_NULL,                AUDIO_P_R_POST_PROCESS_INLET,     AUDIO_P_R_POST_PROCESS_OUTLET     ) \
  OPER1(AUDIO_P_SW_INDEPENDENT_MUTE,  AUDIO_INDEPENDENT_MUTE,  AUDIO_CTL_NULL,                AUDIO_P_SW_POST_PROCESS_INLET,    AUDIO_P_SW_POST_PROCESS_OUTLET    ) \
\
  OPER1(AUDIO_S_DC_FILTER,            AUDIO_NULL_FEATURE,      AUDIO_CTL_NULL,                AUDIO_S_DC_FILTER_INLET,          AUDIO_S_DC_FILTER_OUTLET          ) \
  OPER1(AUDIO_S_SRC_SCALER,           AUDIO_SOURCE_SCALER,     AUDIO_CTL_NULL,                AUDIO_S_SRC_SCALER_INLET,         AUDIO_S_SRC_SCALER_OUTLET         ) \
  OPER1(AUDIO_S_TONE,                 AUDIO_TONE,              AUDIO_CTL_NULL,                AUDIO_S_SRC_SCALER_INLET,         AUDIO_S_SRC_SCALER_OUTLET         ) \
  OPER1(AUDIO_S_VOL,                  AUDIO_MAIN_VOL,          AUDIO_CTL_NULL,                AUDIO_S_SRC_SCALER_INLET,         AUDIO_S_SRC_SCALER_OUTLET         ) \
  OPER1(AUDIO_S_PRESCALER,            AUDIO_PRESCALER,         AUDIO_CTL_NULL,                AUDIO_S_SRC_SCALER_INLET,         AUDIO_S_SRC_SCALER_OUTLET         ) \
  OPER1(AUDIO_S_CHNL_GAIN,            AUDIO_CHNL_GAIN,         AUDIO_CTL_NULL,                AUDIO_S_POST_PROCESS_INLET,       AUDIO_S_POST_PROCESS_OUTLET       ) \
  OPER1(AUDIO_S_BALANCE,              AUDIO_BALANCE,           AUDIO_PRIM_BALANCE_L    OPER2                                                                        \
                                                               AUDIO_PRIM_BALANCE_R,          AUDIO_S_POST_PROCESS_INLET,       AUDIO_S_POST_PROCESS_OUTLET       ) \
  OPER1(AUDIO_S_UPSCALER,             AUDIO_UP_SCALER,         AUDIO_CTL_NULL,                AUDIO_S_POST_PROCESS_INLET,       AUDIO_S_POST_PROCESS_OUTLET       ) \
  OPER1(AUDIO_S_MUTE,                 AUDIO_MUTE,              AUDIO_CTL_NULL,                AUDIO_S_POST_PROCESS_INLET,       AUDIO_S_POST_PROCESS_OUTLET       ) \
  OPER1(AUDIO_S_GPF1_L,               AUDIO_GPF1L,             AUDIO_CTL_NULL,                AUDIO_GPF1_L_INLET,               AUDIO_GPF1_L_OUTLET               ) \
  OPER1(AUDIO_S_GPF1_R,               AUDIO_GPF1R,             AUDIO_CTL_NULL,                AUDIO_GPF1_R_INLET,               AUDIO_GPF1_R_OUTLET               ) \
\
  OPER1(AUDIO_N_SRC_SCALER,           AUDIO_SOURCE_SCALER,     AUDIO_NAV_SRC_SCALER,          AUDIO_N_SRC_SCALER_INLET,         AUDIO_N_BEFORE_EQ_OUTLET          ) \
  OPER1(AUDIO_N_VOICE_FILTER,         AUDIO_PATH_FILTER,       AUDIO_NAV_FILTER_1      OPER2                                                                        \
                                                               AUDIO_NAV_FILTER_2,            AUDIO_N_SRC_SCALER_INLET,         AUDIO_N_BEFORE_EQ_OUTLET          ) \
  OPER1(AUDIO_N_VOL,                  AUDIO_MAIN_VOL,          AUDIO_NAV_VOL,                 AUDIO_N_BACK_IN_INLET,            AUDIO_N_OUT_OUTLET                ) \
  OPER1(AUDIO_N_CHNL_GAIN,            AUDIO_CHNL_GAIN,         AUDIO_NAV_CHNL_GAIN,           AUDIO_N_BACK_IN_INLET,            AUDIO_N_OUT_OUTLET                ) \
  OPER1(AUDIO_N_MUTE,                 AUDIO_MUTE,              AUDIO_NAV_MUTE,                AUDIO_N_BACK_IN_INLET,            AUDIO_N_OUT_OUTLET                ) \
  OPER1(AUDIO_N_FADE,                 AUDIO_FADE,              AUDIO_NAV_FADE_F        OPER2                                                                        \
                                                               AUDIO_NAV_FADE_R,              AUDIO_N_SUPERPOSITION_INLET,      AUDIO_N_OUT_OUTLET                ) \
\
  OPER1(AUDIO_T_SRC_SCALER,           AUDIO_SOURCE_SCALER,     AUDIO_PHONE_SRC_SCALER,        AUDIO_T_SRC_SCALER_IN_INLET,      AUDIO_T_BEFORE_EQ_OUTLET          ) \
  OPER1(AUDIO_T_VOICE_FILTER,         AUDIO_PATH_FILTER,       AUDIO_PHONE_FILTER_1    OPER2                                                                        \
                                                               AUDIO_PHONE_FILTER_2,          AUDIO_T_SRC_SCALER_IN_INLET,      AUDIO_T_BEFORE_EQ_OUTLET          ) \
  OPER1(AUDIO_T_VOL,                  AUDIO_MAIN_VOL,          AUDIO_PHONE_VOL,               AUDIO_T_BACK_IN_INLET,            AUDIO_T_OUT_OUTLET                ) \
  OPER1(AUDIO_T_CHNL_GAIN,            AUDIO_CHNL_GAIN,         AUDIO_PHONE_CHNL_GAIN,         AUDIO_T_BACK_IN_INLET,            AUDIO_T_OUT_OUTLET                ) \
  OPER1(AUDIO_T_MUTE,                 AUDIO_MUTE,              AUDIO_PHONE_MUTE,              AUDIO_T_BACK_IN_INLET,            AUDIO_T_OUT_OUTLET                ) \
  OPER1(AUDIO_T_FADE,                 AUDIO_FADE,              AUDIO_PHONE_FADE_F      OPER2                                                                        \
                                                               AUDIO_PHONE_FADE_R,            AUDIO_T_SUPERPOSITION_INLET,      AUDIO_T_OUT_OUTLET                ) \
\
  OPER1(AUDIO_P_F_SUPERPOSITION,      AUDIO_SUPER_POSITION,    AUDIO_N_ON_FRONT_L_MIX  OPER2                                                                        \
                                                               AUDIO_N_ON_FRONT_R_MIX  OPER2                                                                        \
                                                               AUDIO_T_ON_FRONT_L_MIX  OPER2                                                                        \
                                                               AUDIO_T_ON_FRONT_R_MIX,        AUDIO_P_F_SUPERPOSITION_INLET,    AUDIO_P_F_SUPERPOSITION_OUTLET    ) \
  OPER1(AUDIO_P_R_SUPERPOSITION,      AUDIO_SUPER_POSITION,    AUDIO_N_ON_REAR_L_MIX   OPER2                                                                        \
                                                               AUDIO_N_ON_REAR_R_MIX   OPER2                                                                        \
                                                               AUDIO_T_ON_REAR_L_MIX   OPER2                                                                        \
                                                               AUDIO_T_ON_REAR_R_MIX,         AUDIO_P_R_SUPERPOSITION_INLET,    AUDIO_P_R_SUPERPOSITION_OUTLET    ) \
  OPER1(AUDIO_P_SUB_SUPERPOSITION,    AUDIO_SUPER_POSITION,    AUDIO_CTL_NULL,                AUDIO_P_SUB_SUPERPOSITION_INLET,  AUDIO_P_SUB_SUPERPOSITION_OUTLET  ) \
  OPER1(AUDIO_S_SUPERPOSITION,        AUDIO_SUPER_POSITION,    AUDIO_CTL_NULL,                AUDIO_S_SUPERPOSITION_INLET,      AUDIO_S_SUPERPOSITION_OUTLET      )

//////////////////////////////////////////////////////////////////////////////
//name, modules
#define AUDIO_PHYSICAL_CHNL_TBL \
  OPER1(AUDIO_PRIM_FRONT_PHYSICAL_CHNL,  AUDIO_P_DC_FILTER           OPER2     \
                                         AUDIO_P_SRC_SCALER          OPER2     \
                                         AUDIO_P_COMPRESSOR          OPER2     \
                                         AUDIO_P_GEQ                 OPER2     \
                                         AUDIO_P_DOWN_SCALER         OPER2     \
                                         AUDIO_P_TONE                OPER2     \
                                         AUDIO_P_MAIN_VOL            OPER2     \
                                         AUDIO_P_LOUDNESS            OPER2     \
                                         AUDIO_P_F_SCALER            OPER2     \
                                         AUDIO_P_FR_PEQ              OPER2     \
                                         AUDIO_P_FL_PEQ              OPER2     \
                                         AUDIO_P_F_DELAY             OPER2     \
                                         AUDIO_P_F_CHNL_GAIN         OPER2     \
                                         AUDIO_P_F_BALANCE           OPER2     \
                                         AUDIO_P_F_FADER             OPER2     \
                                         AUDIO_P_F_UPSCALER          OPER2     \
                                         AUDIO_P_F_MUTE              OPER2     \
                                         AUDIO_P_F_INDEPENDENT_MUTE  OPER2     \
                                         AUDIO_P_F_SUPERPOSITION            )  \
\
  OPER1(AUDIO_PRIM_REAR_PHYSICAL_CHNL,   AUDIO_P_DC_FILTER           OPER2     \
                                         AUDIO_P_SRC_SCALER          OPER2     \
                                         AUDIO_P_COMPRESSOR          OPER2     \
                                         AUDIO_P_GEQ                 OPER2     \
                                         AUDIO_P_DOWN_SCALER         OPER2     \
                                         AUDIO_P_TONE                OPER2     \
                                         AUDIO_P_MAIN_VOL            OPER2     \
                                         AUDIO_P_LOUDNESS            OPER2     \
                                         AUDIO_P_R_SCALER            OPER2     \
                                         AUDIO_P_RR_PEQ              OPER2     \
                                         AUDIO_P_RL_PEQ              OPER2     \
                                         AUDIO_P_R_DELAY             OPER2     \
                                         AUDIO_P_R_CHNL_GAIN         OPER2     \
                                         AUDIO_P_R_BALANCE           OPER2     \
                                         AUDIO_P_R_FADER             OPER2     \
                                         AUDIO_P_R_UPSCALER          OPER2     \
                                         AUDIO_P_R_MUTE              OPER2     \
                                         AUDIO_P_R_INDEPENDENT_MUTE  OPER2     \
                                         AUDIO_P_R_SUPERPOSITION            )  \
\
  OPER1(AUDIO_NAV_PHYSICAL_CHNL,         AUDIO_N_SRC_SCALER          OPER2     \
                                         AUDIO_N_VOICE_FILTER        OPER2     \
                                         AUDIO_N_VOL                 OPER2     \
                                         AUDIO_N_CHNL_GAIN           OPER2     \
                                         AUDIO_N_MUTE                OPER2     \
                                         AUDIO_N_FADE                OPER2  )  \
\
  OPER1(AUDIO_PHONE_PHYSICAL_CHNL,       AUDIO_T_SRC_SCALER          OPER2     \
                                         AUDIO_T_VOICE_FILTER        OPER2     \
                                         AUDIO_T_VOL                 OPER2     \
                                         AUDIO_T_CHNL_GAIN           OPER2     \
                                         AUDIO_T_MUTE                OPER2     \
                                         AUDIO_T_FADE                       )

//////////////////////////////////////////////////////////////////////////////
//name, inlet
#define AUDIO_PHYSICAL_OUTPUT_TBL \
  OPER1(AUDIO_HOST_IIS_OUT0,  AUDIO_HOST_IIS_OUT0_INLET  ) \
  OPER1(AUDIO_HOST_IIS_OUT1,  AUDIO_HOST_IIS_OUT1_INLET  ) \
  OPER1(AUDIO_FRONT_DAC_OUT,  AUDIO_FRONT_DAC_OUT_INLET  ) \
  OPER1(AUDIO_REAR_DAC_OUT,   AUDIO_REAR_DAC_OUT_INLET   )

//////////////////////////////////////////////////////////////////////////////
//name, pid, format, P_S_Value, T_N_Value
#define AUDIO_NEA (~0)

#define AUDIO_PHYSICAL_INPUT_TBL \
  OPER1(AUDIO_RADIO,            AUDIO_NEA,  AUDIO_NEA,                     0x00,       AUDIO_NEA                               ) \
  OPER1(AUDIO_AIN0,             0x00,       AUDIO_DIFFERENTIAL_1VRMS,      0x08,       AUDIO_NEA                               ) \
  OPER1(AUDIO_AIN1,             0x01,       AUDIO_HIGH_COMMON_MODE_2VRMS,  0x09,       AUDIO_NEA                               ) \
  OPER1(AUDIO_AIN2,             0x02,       AUDIO_DIFFERENTIAL_2VRMS,      0x0A,       AUDIO_EASYP_SrcSw_AIN2onPhone           ) \
  OPER1(AUDIO_AIN3,             0x03,       AUDIO_DIFFERENTIAL_2VRMS,      AUDIO_NEA,  AUDIO_EASYP_SrcSw_AIN3onNav             ) \
  OPER1(AUDIO_IIS0,             0x0A,       AUDIO_PHILIPS_I2S,             0x10,       AUDIO_REL_ADDR(AUDIO_X_InputFlagIIS0)   ) \
  OPER1(AUDIO_IIS1,             0x0B,       AUDIO_PHILIPS_I2S,             0x11,       AUDIO_REL_ADDR(AUDIO_X_InputFlagIIS1)   ) \
  OPER1(AUDIO_IIS2,             0x0C,       AUDIO_PHILIPS_I2S,             0x12,       AUDIO_REL_ADDR(AUDIO_X_InputFlagIIS2)   ) \
  OPER1(AUDIO_HOST_IIS0,        0x15,       AUDIO_PHILIPS_I2S,             0x13,       AUDIO_REL_ADDR(AUDIO_X_HIIS0InL)        ) \
  OPER1(AUDIO_HOST_IIS1,        0x15,       AUDIO_PHILIPS_I2S,             0x14,       AUDIO_REL_ADDR(AUDIO_X_HIIS1InL)        ) \
  OPER1(AUDIO_SPDIF,            0x1E,       AUDIO_DIGITAL_SPDIF,           0x15,       AUDIO_REL_ADDR(AUDIO_X_InputFlagSPDIF)  ) \
  OPER1(AUDIO_NOISE_GENERATOR,  AUDIO_NEA,  AUDIO_NEA,                     0x1E,       AUDIO_NEA                               ) \
  OPER1(AUDIO_SINE_GENERATOR,   AUDIO_NEA,  AUDIO_NEA,                     0x1F,       AUDIO_NEA                               )

//////////////////////////////////////////////////////////////////////////////
//physical channel, speaker name, speaker channel
#define AUDIO_MAP_PHYSICAL_CHNL_TO_SPK_TBL\
  OPER1(AUDIO_PRIM_FRONT_PHYSICAL_CHNL,   AUDIO_FRONT_LEFT_SPK,   AUDIO_SPK_LEFT_CHNL   ) \
  OPER1(AUDIO_PRIM_FRONT_PHYSICAL_CHNL,   AUDIO_FRONT_RIGHT_SPK,  AUDIO_SPK_RIGHT_CHNL ) \
  OPER1(AUDIO_PRIM_REAR_PHYSICAL_CHNL,    AUDIO_REAR_LEFT_SPK,    AUDIO_SPK_LEFT_CHNL   ) \
  OPER1(AUDIO_PRIM_REAR_PHYSICAL_CHNL,    AUDIO_REAR_RIGHT_SPK,   AUDIO_SPK_RIGHT_CHNL  )

#endif
