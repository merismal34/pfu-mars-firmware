/* Renesas/Hitachi H8S/2318 Series register definitions (HEW project generator, 2318s.h). */
#ifndef H8S_2318S_H
#define H8S_2318S_H
/************************************************************************/
/*      H8S/2318 Series Include File                       Ver 1.1      */
/************************************************************************/
struct st_tpu {                                         /* struct TPU   */
              union {                                   /* TSTR         */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char wk  :2;        /*              */
                           unsigned char CST5:1;        /*    CST5      */
                           unsigned char CST4:1;        /*    CST4      */
                           unsigned char CST3:1;        /*    CST3      */
                           unsigned char CST2:1;        /*    CST2      */
                           unsigned char CST1:1;        /*    CST1      */
                           unsigned char CST0:1;        /*    CST0      */
                           }      BIT;                  /*              */
                    }           TSTR;                   /*              */
              union {                                   /* TSYR         */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char wk   :2;       /*              */
                           unsigned char SYNC5:1;       /*    SYNC5     */
                           unsigned char SYNC4:1;       /*    SYNC4     */
                           unsigned char SYNC3:1;       /*    SYNC3     */
                           unsigned char SYNC2:1;       /*    SYNC2     */
                           unsigned char SYNC1:1;       /*    SYNC1     */
                           unsigned char SYNC0:1;       /*    SYNC0     */
                           }      BIT;                  /*              */
                    }           TSYR;                   /*              */
};                                                      /*              */
struct st_tpu0 {                                        /* struct TPU0  */
               union {                                  /* TCR          */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char CCLR:3;       /*    CCLR      */
                            unsigned char CKEG:2;       /*    CKEG      */
                            unsigned char TPSC:3;       /*    TPSC      */
                            }      BIT;                 /*              */
                     }          TCR;                    /*              */
               union {                                  /* TMDR         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk :2;        /*              */
                            unsigned char BFB:1;        /*    BFB       */
                            unsigned char BFA:1;        /*    BFA       */
                            unsigned char MD :4;        /*    MD        */
                            }      BIT;                 /*              */
                     }          TMDR;                   /*              */
               union {                                  /* TIOR         */
                     unsigned int WORD;                 /*  Word Access */
                     struct {                           /*  Byte Access */
                            unsigned char H;            /*    TIORH     */
                            unsigned char L;            /*    TIORL     */
                            }     BYTE;                 /*              */
                     struct {                           /*  Bit  Access */
                            unsigned char IOB:4;        /*    IOB       */
                            unsigned char IOA:4;        /*    IOA       */
                            unsigned char IOD:4;        /*    IOD       */
                            unsigned char IOC:4;        /*    IOC       */
                            }     BIT;                  /*              */
                     }          TIOR;                   /*              */
               union {                                  /* TIER         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char TTGE :1;      /*    TTGE      */
                            unsigned char      :2;      /*              */
                            unsigned char TCIEV:1;      /*    TCIEV     */
                            unsigned char TGIED:1;      /*    TGIED     */
                            unsigned char TGIEC:1;      /*    TGIEC     */
                            unsigned char TGIEB:1;      /*    TGIEB     */
                            unsigned char TGIEA:1;      /*    TGIEA     */
                            }      BIT;                 /*              */
                     }          TIER;                   /*              */
               union {                                  /* TSR0         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :3;       /*              */
                            unsigned char TCFV:1;       /*    TCFV      */
                            unsigned char TGFD:1;       /*    TGFD      */
                            unsigned char TGFC:1;       /*    TGFC      */
                            unsigned char TGFB:1;       /*    TGFB      */
                            unsigned char TGFA:1;       /*    TGFA      */
                            }      BIT;                 /*              */
                     }          TSR;                    /*              */
               unsigned int     TCNT;                   /* TCNT         */
               unsigned int     TGRA;                   /* TGRA         */
               unsigned int     TGRB;                   /* TGRB         */
               unsigned int     TGRC;                   /* TGRC         */
               unsigned int     TGRD;                   /* TGRD         */
};                                                      /*              */
struct st_tpu1 {                                        /* struct TPU1  */
               union {                                  /* TCR          */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char CCLR:2;       /*    CCLR      */
                            unsigned char CKEG:2;       /*    CKEG      */
                            unsigned char TPSC:3;       /*    TPSC      */
                            }      BIT;                 /*              */
                     }          TCR;                    /*              */
               union {                                  /* TMDR         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk:4;         /*              */
                            unsigned char MD:4;         /*    MD        */
                            }      BIT;                 /*              */
                     }          TMDR;                   /*              */
               union {                                  /* TIOR         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char IOB:4;        /*    IOB       */
                            unsigned char IOA:4;        /*    IOA       */
                            }      BIT;                 /*              */
                     }          TIOR;                   /*              */
               char             wk;                     /*              */
               union {                                  /* TIER         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char TTGE :1;      /*    TTGE      */
                            unsigned char      :1;      /*              */
                            unsigned char TCIEU:1;      /*    TCIEU     */
                            unsigned char TCIEV:1;      /*    TCIEV     */
                            unsigned char      :2;      /*              */
                            unsigned char TGIEB:1;      /*    TGIEB     */
                            unsigned char TGIEA:1;      /*    TGIEA     */
                            }      BIT;                 /*              */
                     }          TIER;                   /*              */
               union {                                  /* TSR1         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char TCFD:1;       /*    TCFD      */
                            unsigned char     :1;       /*              */
                            unsigned char TCFU:1;       /*    TCFU      */
                            unsigned char TCFV:1;       /*    TCFV      */
                            unsigned char     :2;       /*              */
                            unsigned char TGFB:1;       /*    TGFB      */
                            unsigned char TGFA:1;       /*    TGFA      */
                            }      BIT;                 /*              */
                     }          TSR;                    /*              */
               unsigned int     TCNT;                   /* TCNT         */
               unsigned int     TGRA;                   /* TGRA         */
               unsigned int     TGRB;                   /* TGRB         */
};                                                      /*              */
struct st_p1 {                                          /* struct P1    */
             unsigned char      DDR;                    /* P1DDR        */
             char               wk1[159];               /*              */
             union {                                    /* P1PORT       */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            PORT;                   /*              */
             char               wk2[15];                /*              */
             union {                                    /* P1DR         */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            DR;                     /*              */
};                                                      /*              */
struct st_p3 {                                          /* struct P3    */
             unsigned char      DDR;                    /* P3DDR        */
             char               wk1[159];               /*              */
             union {                                    /* P3PORT       */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char wk:2;           /*    Bit 7,6   */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            PORT;                   /*              */
             char               wk2[15];                /*              */
             union {                                    /* P3DR         */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char wk:2;           /*    Bit 7,6   */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            DR;                     /*              */
             char               wk3[19];                /*              */
             union {                                    /* P3ODR        */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char wk:2;           /*    Bit 7,6   */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            ODR;                    /*              */
};                                                      /*              */
struct st_p4 {                                          /* struct P4    */
             union {                                    /* P4PORT       */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            PORT;                   /*              */
};                                                      /*              */
struct st_pa {                                          /* struct PA    */
             unsigned char      DDR;                    /* PADDR        */
             char               wk1[159];               /*              */
             union {                                    /* PAPORT       */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            PORT;                   /*              */
             char               wk2[15];                /*              */
             union {                                    /* PADR         */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            DR;                     /*              */
             char               wk3[6];                 /*              */
             union {                                    /* PAPCR        */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            PCR;                    /*              */
             char               wk4[6];                 /*              */
             union {                                    /* PAODR        */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            ODR;                    /*              */
};                                                      /*              */
struct st_pb {                                          /* struct PB    */
             unsigned char      DDR;                    /* PBDDR        */
             char               wk1[159];               /*              */
             union {                                    /* PBPORT       */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            PORT;                   /*              */
             char               wk2[15];                /*              */
             union {                                    /* PBDR         */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            DR;                     /*              */
             char               wk3[6];                 /*              */
             union {                                    /* PBPCR        */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char B7:1;           /*    Bit 7     */
                          unsigned char B6:1;           /*    Bit 6     */
                          unsigned char B5:1;           /*    Bit 5     */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            PCR;                    /*              */
};                                                      /*              */
struct st_pg {                                          /* struct PG    */
             unsigned char      DDR;                    /* PGDDR        */
             char               wk1[159];               /*              */
             union {                                    /* PGPORT       */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char wk:3;           /*    Bit 7,6,5 */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            PORT;                   /*              */
             char               wk2[15];                /*              */
             union {                                    /* PGDR         */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char wk:3;           /*    Bit 7,6,5 */
                          unsigned char B4:1;           /*    Bit 4     */
                          unsigned char B3:1;           /*    Bit 3     */
                          unsigned char B2:1;           /*    Bit 2     */
                          unsigned char B1:1;           /*    Bit 1     */
                          unsigned char B0:1;           /*    Bit 0     */
                          }      BIT;                   /*              */
                   }            DR;                     /*              */
};                                                      /*              */
struct st_intc {                                        /* struct INTC  */
               union {                                  /* IPRA         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    IRQ0      */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    IRQ1      */
                            }      BIT;                 /*              */
                     }          IPRA;                   /*              */
               union {                                  /* IPRB         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    IRQ2,3    */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    IRQ4,5    */
                            }      BIT;                 /*              */
                     }          IPRB;                   /*              */
               union {                                  /* IPRC         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    IRQ6,IRQ7 */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    DTC       */
                            }      BIT;                 /*              */
                     }          IPRC;                   /*              */
               union {                                  /* IPRD         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /* IPRD WDT     */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /* IPRD REFRESH */
                            }      BIT;                 /*              */
                     }          IPRD;                   /*              */
               union {                                  /* IPRE         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    IPR6-4    */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    IPR2-0    */
                            }      BIT;                 /*              */
                     }          IPRE;                   /*              */
               union {                                  /* IPRF         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    TPU0      */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    TPU1      */
                            }      BIT;                 /*              */
                     }          IPRF;                   /*              */
               union {                                  /* IPRG         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    TPU2      */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    TPU3      */
                            }      BIT;                 /*              */
                     }          IPRG;                   /*              */
               union {                                  /* IPRH         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    TPU4      */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    TPU5      */
                            }      BIT;                 /*              */
                     }          IPRH;                   /*              */
               union {                                  /* IPRI         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    TMR0      */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    TMR1      */
                            }      BIT;                 /*              */
                     }          IPRI;                   /*              */
               union {                                  /* IPRJ         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*   IPR6-4     */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*   IPR2-0     */
                            }      BIT;                 /*              */
                     }          IPRJ;                   /*              */
               union {                                  /* IPRK         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char wk  :1;       /*              */
                            unsigned char HIGH:3;       /*    SCI1      */
                            unsigned char     :1;       /*              */
                            unsigned char LOW :3;       /*    SCI2      */
                            }      BIT;                 /*              */
                     }          IPRK;                   /*              */
               char             wk[93];                 /*              */
               union {                                  /* ISCR         */
                     unsigned int WORD;                 /*  Word Access */
                     struct {                           /*  Byte Access */
                            unsigned char H;            /*    ISCRH     */
                            unsigned char L;            /*    ISCRL     */
                            }     BYTE;                 /*              */
                     struct {                           /*  Bit  Access */
                            unsigned char IRQ7SC:2;     /*    IRQ7SC    */
                            unsigned char IRQ6SC:2;     /*    IRQ6SC    */
                            unsigned char IRQ5SC:2;     /*    IRQ5SC    */
                            unsigned char IRQ4SC:2;     /*    IRQ4SC    */
                            unsigned char IRQ3SC:2;     /*    IRQ3SC    */
                            unsigned char IRQ2SC:2;     /*    IRQ2SC    */
                            unsigned char IRQ1SC:2;     /*    IRQ1SC    */
                            unsigned char IRQ0SC:2;     /*    IRQ0SC    */
                            }     BIT;                  /*              */
                     }          ISCR;                   /*              */
               union {                                  /* IER          */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char IRQ7E:1;      /*    IRQ7E     */
                            unsigned char IRQ6E:1;      /*    IRQ6E     */
                            unsigned char IRQ5E:1;      /*    IRQ5E     */
                            unsigned char IRQ4E:1;      /*    IRQ4E     */
                            unsigned char IRQ3E:1;      /*    IRQ3E     */
                            unsigned char IRQ2E:1;      /*    IRQ2E     */
                            unsigned char IRQ1E:1;      /*    IRQ1E     */
                            unsigned char IRQ0E:1;      /*    IRQ0E     */
                            }      BIT;                 /*              */
                     }          IER;                    /*              */
               union {                                  /* ISR          */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char IRQ7F:1;      /*    IRQ7F     */
                            unsigned char IRQ6F:1;      /*    IRQ6F     */
                            unsigned char IRQ5F:1;      /*    IRQ5F     */
                            unsigned char IRQ4F:1;      /*    IRQ4F     */
                            unsigned char IRQ3F:1;      /*    IRQ3F     */
                            unsigned char IRQ2F:1;      /*    IRQ2F     */
                            unsigned char IRQ1F:1;      /*    IRQ1F     */
                            unsigned char IRQ0F:1;      /*    IRQ0F     */
                            }      BIT;                 /*              */
                     }          ISR;                    /*              */
};                                                      /*              */
struct st_bsc {                                         /* struct BSC   */
              union {                                   /* ABWCR        */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char B7:1;          /*    Bit 7     */
                           unsigned char B6:1;          /*    Bit 6     */
                           unsigned char B5:1;          /*    Bit 5     */
                           unsigned char B4:1;          /*    Bit 4     */
                           unsigned char B3:1;          /*    Bit 3     */
                           unsigned char B2:1;          /*    Bit 2     */
                           unsigned char B1:1;          /*    Bit 1     */
                           unsigned char B0:1;          /*    Bit 0     */
                           }      BIT;                  /*              */
                    }           ABWCR;                  /*              */
              union {                                   /* ASTCR        */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char B7:1;          /*    Bit 7     */
                           unsigned char B6:1;          /*    Bit 6     */
                           unsigned char B5:1;          /*    Bit 5     */
                           unsigned char B4:1;          /*    Bit 4     */
                           unsigned char B3:1;          /*    Bit 3     */
                           unsigned char B2:1;          /*    Bit 2     */
                           unsigned char B1:1;          /*    Bit 1     */
                           unsigned char B0:1;          /*    Bit 0     */
                           }      BIT;                  /*              */
                    }           ASTCR;                  /*              */
              union {                                   /* WCR          */
                    unsigned int WORD;                  /*  Word Access */
                    struct {                            /*  Byte Access */
                           unsigned char H;             /*    WCRH      */
                           unsigned char L;             /*    WCRL      */
                           }     BYTE;                  /*              */
                    struct {                            /*  Bit  Access */
                           unsigned char W7:2;          /*    W7        */
                           unsigned char W6:2;          /*    W6        */
                           unsigned char W5:2;          /*    W5        */
                           unsigned char W4:2;          /*    W4        */
                           unsigned char W3:2;          /*    W3        */
                           unsigned char W2:2;          /*    W2        */
                           unsigned char W1:2;          /*    W1        */
                           unsigned char W0:2;          /*    W0        */
                           }     BIT;                   /*              */
                    }           WCR;                    /*              */
              union {                                   /* BCR          */
                    unsigned int WORD;                  /*  Word Access */
                    struct {                            /*  Byte Access */
                           unsigned char H;             /*    BCRH      */
                           unsigned char L;             /*    BCRL      */
                           }     BYTE;                  /*              */
                    struct {                            /*  Bit  Access */
                           unsigned char ICIS1 :1;      /*    ICIS1     */
                           unsigned char ICIS0 :1;      /*    ICIS0     */
                           unsigned char BRSTRM:1;      /*    BRSTRM    */
                           unsigned char BRSTS1:1;      /*    BRSTS1    */
                           unsigned char BRSTS0:1;      /*    BRSTS0    */
			   unsigned char RMTS  :3;      /*    RMTS      */
                           unsigned char BRLE  :1;      /*    BRLE      */
                           unsigned char BREQOE:1;      /*    BREQOE    */
                           unsigned char EAE   :1;      /*    EAE       */
                           unsigned char       :1;      /*              */
                           unsigned char  DDS  :1;      /*    DDS       */
                           unsigned char       :1;      /*              */
                           unsigned char WDBE  :1;      /*    WDBE      */
                           unsigned char WAITE :1;      /*    WAITE     */
                           }     BIT;                   /*              */
                    }           BCR;                    /*              */
              char              wk1[111];               /*              */
              union {                                   /* PFCR1        */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char        :4;     /*              */
                           unsigned char A23E   :1;     /*    A23E      */
                           unsigned char A22E   :1;     /*    A22E      */
                           unsigned char A21E   :1;     /*    A21E      */
                           unsigned char A20E   :1;     /*    A20E      */
                           }      BIT;                  /*              */
                    }           PFCR1;                  /*              */
              char              wk2[102];               /*              */
              union {                                   /* PFCR2        */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char WAITPS :1;     /*    WAITPS    */
                           unsigned char BREQOPS:1;     /*    BREQOPS   */
                           unsigned char CS167E :1;     /*    CS167E    */
                           unsigned char CS25E  :1;     /*    CS25E     */
                           unsigned char ASOD   :1;     /*    ASOD      */
                           unsigned char        :3;     /*              */
                           }      BIT;                  /*              */
                    }           PFCR2;                  /*              */
};                                                      /*              */
struct st_dtc {                                         /* struct DTC   */
              union {                                   /* EA           */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char B7:1;          /*    IRQ0      */
                           unsigned char B6:1;          /*    IRQ1      */
                           unsigned char B5:1;          /*    IRQ2      */
                           unsigned char B4:1;          /*    IRQ3      */
                           unsigned char B3:1;          /*    IRQ4      */
                           unsigned char B2:1;          /*    IRQ5      */
                           unsigned char B1:1;          /*    IRQ6      */
                           unsigned char B0:1;          /*    IRQ7      */
                           }      BIT;                  /*              */
                    }           EA;                     /*              */
              union {                                   /* EB           */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char wk:1;          /*              */
                           unsigned char B6:1;          /*    ADI       */
                           unsigned char B5:1;          /*    TGI0A     */
                           unsigned char B4:1;          /*    TGI0B     */
                           unsigned char B3:1;          /*    TGI0C     */
                           unsigned char B2:1;          /*    TGI0D     */
                           unsigned char B1:1;          /*    TGI1A     */
                           unsigned char B0:1;          /*    TGI1B     */
                           }      BIT;                  /*              */
                    }           EB;                     /*              */
              union {                                   /* EC           */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char B7:1;          /*    TGI2A     */
                           unsigned char B6:1;          /*    TGI2B     */
                           unsigned char B5:1;          /*    TGI3A     */
                           unsigned char B4:1;          /*    TGI3B     */
                           unsigned char B3:1;          /*    TGI3C     */
                           unsigned char B2:1;          /*    TGI3D     */
                           unsigned char B1:1;          /*    TGI4A     */
                           unsigned char B0:1;          /*    TGI4B     */
                           }      BIT;                  /*              */
                    }           EC;                     /*              */
              union {                                   /* ED           */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char wk:2;          /*              */
                           unsigned char B5:1;          /*    TGI5A     */
                           unsigned char B4:1;          /*    TGI5B     */
                           unsigned char B3:1;          /*    CMI0A     */
                           unsigned char B2:1;          /*    CMI0B     */
                           unsigned char B1:1;          /*    CMI1A     */
                           unsigned char B0:1;          /*    CMI1B     */
                           }      BIT;                  /*              */
                    }           ED;                     /*              */
              union {                                   /* EE           */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char wk:4;          /*              */
                           unsigned char B3:1;          /*    RXI0      */
                           unsigned char B2:1;          /*    TXI0      */
                           unsigned char B1:1;          /*    RXI1      */
                           unsigned char B0:1;          /*    TXI1      */
                           }      BIT;                  /*              */
                    }           EE;                     /*              */
              char              wk[2];                  /*              */
              union {                                   /* VECR         */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char SWDTE:1;       /*    SWDTE     */
                           unsigned char DTVEC:7;       /*    DTVEC     */
                           }      BIT;                  /*              */
                    }           VECR;                   /*              */
};                                                      /*              */
union un_sbycr {                                        /* union SBYCR  */
               unsigned char BYTE;                      /*  Byte Access */
               struct {                                 /*  Bit  Access */
                      unsigned char SSBY  :1;           /*    SSBY      */
                      unsigned char STS   :3;           /*    STS       */
                      unsigned char OPE   :1;           /*    OPE       */
                      unsigned char       :2;           /*              */
                      unsigned char IRQ37S:1;           /*    IRQ37S    */
                      }      BIT;                       /*              */
};                                                      /*              */
union un_syscr {                                        /* union SYSCR  */
               unsigned char BYTE;                      /*  Byte Access */
               struct {                                 /*  Bit  Access */
                      unsigned char wk    :2;           /*              */
                      unsigned char INTM  :2;           /*    INTM      */
                      unsigned char NMIEG :1;           /*    NMIEG     */
                      unsigned char LWROD :1;           /*    LWROD     */
                      unsigned char IRQPAS:1;           /*    IRQPAS    */
                      unsigned char RAME  :1;           /*    RAME      */
                      }      BIT;                       /*              */
};                                                      /*              */
union un_sckcr {                                        /* union SCKCR  */
               unsigned char BYTE;                      /*  Byte Access */
               struct {                                 /*  Bit  Access */
                      unsigned char PSTOP:1;            /*    PSTOP     */
                      unsigned char      :1;            /*              */
                      unsigned char DIV  :1;            /*    DIV       */
                      unsigned char      :2;            /*              */
                      unsigned char SCK  :3;            /*    SCK       */
                      }      BIT;                       /*              */
};                                                      /*              */
union un_mdcr {                                         /* union MDCR   */
              unsigned char     BYTE;                   /*  Byte Access */
              struct {                                  /*  Bit  Access */
                     unsigned char wk :5;               /*              */
                     unsigned char MDS:3;               /*    MDS       */
                     }          BIT;                    /*              */
};                                                      /*              */
union un_mstpcr {                                       /* union MSTPCR */
                unsigned int WORD;                      /*  Word Access */
                struct {                                /*  Byte Access */
                       unsigned char H;                 /*    MSTPCRH   */
                       unsigned char L;                 /*    MSTPCRL   */
                       }     BYTE;                      /*              */
                struct {                                /*  Bit  Access */
                       unsigned char B15:1;             /*DMAC Stop Mode*/
                       unsigned char B14:1;             /*DTC Stop Mode */
                       unsigned char B13:1;             /*TPU Stop Mode */
                       unsigned char B12:1;             /*TMR Stop Mode */
                       unsigned char B11:1;             /*PPG Stop Mode */
                       unsigned char B10:1;             /*D/A Channel0,1*/
                       unsigned char B9 :1;             /*A/D Stop Mode */
                       unsigned char    :1;             /*              */
                       unsigned char B7 :1;             /*SCI2 Stop Mode*/
                       unsigned char B6 :1;             /*SCI1 Stop Mode*/
                       unsigned char B5 :1;             /*SCI0 Stop Mode*/
                       unsigned char B4 :1;             /*D/A Channel2,3*/
                       unsigned char    :4;             /*              */
                       }     BIT;                       /*              */
};                                                      /*              */
union un_syscr2 {                                       /* union SYSCR2 */
                unsigned char BYTE;                     /*  Byte Access */
                struct {                                /*  Bit  Access */
                       unsigned char wk   :4;           /*              */
                       unsigned char FLSHE:1;           /*    FLSHE     */
                       unsigned char      :3;           /*              */
                       }      BIT;                      /*              */
};                                                      /*              */
struct st_sci {                                         /* struct SCI   */
              union {                                   /* SMR          */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char CA  :1;        /*    C/A       */
                           unsigned char CHR :1;        /*    CHR       */
                           unsigned char PE  :1;        /*    PE        */
                           unsigned char OE  :1;        /*    O/E       */
                           unsigned char STOP:1;        /*    STOP      */
                           unsigned char MP  :1;        /*    MP        */
                           unsigned char CKS :2;        /*    CKS       */
                           }      BIT;                  /*              */
                    }           SMR;                    /*              */
              unsigned char     BRR;                    /* BRR          */
              union {                                   /* SCR          */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char TIE :1;        /*    TIE       */
                           unsigned char RIE :1;        /*    RIE       */
                           unsigned char TE  :1;        /*    TE        */
                           unsigned char RE  :1;        /*    RE        */
                           unsigned char MPIE:1;        /*    MPIE      */
                           unsigned char TEIE:1;        /*    TEIE      */
                           unsigned char CKE :2;        /*    CKE       */
                           }      BIT;                  /*              */
                    }           SCR;                    /*              */
              unsigned char     TDR;                    /* TDR          */
              union {                                   /* SSR          */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char TDRE:1;        /*    TDRE      */
                           unsigned char RDRF:1;        /*    RDRF      */
                           unsigned char ORER:1;        /*    ORER      */
                           unsigned char FER :1;        /*    FER       */
                           unsigned char PER :1;        /*    PER       */
                           unsigned char TEND:1;        /*    TEND      */
                           unsigned char MPB :1;        /*    MPB       */
                           unsigned char MPBT:1;        /*    MPBT      */
                           }      BIT;                  /*              */
                    }           SSR;                    /*              */
              unsigned char     RDR;                    /* RDR          */
              union {                                   /* SCMR         */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char wk  :4;        /*              */
                           unsigned char SDIR:1;        /*    SDIR      */
                           unsigned char SINV:1;        /*    SINV      */
                           unsigned char     :1;        /*              */
                           unsigned char SMIF:1;        /*    SMIF      */
                           }      BIT;                  /*              */
                    }           SCMR;                   /*              */
              char              wk;                     /*              */
};                                                      /*              */
struct st_ici {                                         /* struct ICI   */
              union {                                   /* SMR          */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char GM :1;         /*    GM        */
                           unsigned char BLK:1;         /*    BLK       */
                           unsigned char PE :1;         /*    PE        */
                           unsigned char OE :1;         /*    O/E       */
                           unsigned char BCP:2;         /*    BCP       */
                           unsigned char CKS:2;         /*    CKS       */
                           }      BIT;                  /*              */
                    }           SMR;                    /*              */
              unsigned char     BRR;                    /* BRR          */
              union {                                   /* SCR          */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char TIE :1;        /*    TIE       */
                           unsigned char RIE :1;        /*    RIE       */
                           unsigned char TE  :1;        /*    TE        */
                           unsigned char RE  :1;        /*    RE        */
                           unsigned char MPIE:1;        /*    MPIE      */
                           unsigned char TEIE:1;        /*    TEIE      */
                           unsigned char CKE :2;        /*    CKE       */
                           }      BIT;                  /*              */
                    }           SCR;                    /*              */
              unsigned char     TDR;                    /* TDR          */
              union {                                   /* SSR          */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char TDRE:1;        /*    TDRE      */
                           unsigned char RDRF:1;        /*    RDRF      */
                           unsigned char ORER:1;        /*    ORER      */
                           unsigned char ERS :1;        /*    ERS       */
                           unsigned char PER :1;        /*    PER       */
                           unsigned char TEND:1;        /*    TEND      */
                           unsigned char MPB :1;        /*    MPB       */
                           unsigned char MPBT:1;        /*    MPBT      */
                           }      BIT;                  /*              */
                    }           SSR;                    /*              */
              unsigned char     RDR;                    /* RDR          */
              union {                                   /* SCMR         */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char wk  :4;        /*              */
                           unsigned char SDIR:1;        /*    SDIR      */
                           unsigned char SINV:1;        /*    SINV      */
                           unsigned char     :1;        /*              */
                           unsigned char SMIF:1;        /*    SMIF      */
                           }      BIT;                  /*              */
                    }           SCMR;                   /*              */
              char              wk;                     /*              */
};                                                      /*              */
struct st_ad {                                          /* struct A/D   */
             unsigned int       DRA;                    /* ADDRA        */
             unsigned int       DRB;                    /* ADDRB        */
             unsigned int       DRC;                    /* ADDRC        */
             unsigned int       DRD;                    /* ADDRD        */
             union {                                    /* ADCSR        */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char ADF :1;         /*    ADF       */
                          unsigned char ADIE:1;         /*    ADIE      */
                          unsigned char ADST:1;         /*    ADST      */
                          unsigned char SCAN:1;         /*    SCAN      */
                          unsigned char CKS :1;         /*    CKS       */
                          unsigned char CH  :3;         /*    CH        */
                          }      BIT;                   /*              */
                   }            CSR;                    /*              */
             union {                                    /* ADCR         */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char TRGS:2;         /*    TRGS      */
                          unsigned char     :2;         /*              */
                          unsigned char CKS :1;         /*    CKS       */
                          unsigned char CH3 :1;         /*    CH3       */
                          unsigned char     :2;         /*              */
                          }      BIT;                   /*              */
                   }            CR;                     /*              */
};                                                      /*              */
struct st_da {                                          /* struct D/A   */
             unsigned char      DR0;                    /* DADR0        */
             unsigned char      DR1;                    /* DADR1        */
             union {                                    /* DACR         */
                   unsigned char BYTE;                  /*  Byte Access */
                   struct {                             /*  Bit  Access */
                          unsigned char DAOE1:1;        /*    DAOE1     */
                          unsigned char DAOE0:1;        /*    DAOE0     */
                          unsigned char DAE  :1;        /*    DAE       */
                          unsigned char      :5;        /*              */
                          }      BIT;                   /*              */
                   }            CR;                     /*              */
};                                                      /*              */
struct st_tmr {                                         /* struct TMR   */
              union {                                   /* TCR0         */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char CMIEB:1;       /*    CMIEB     */
                           unsigned char CMIEA:1;       /*    CMIEA     */
                           unsigned char OVIE :1;       /*    OVIE      */
                           unsigned char CCLR :2;       /*    CCLR      */
                           unsigned char CKS  :3;       /*    CKS       */
                           }      BIT;                  /*              */
                    }           TCR0;                   /*              */
              union {                                   /* TCR1         */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char CMIEB:1;       /*    CMIEB     */
                           unsigned char CMIEA:1;       /*    CMIEA     */
                           unsigned char OVIE :1;       /*    OVIE      */
                           unsigned char CCLR :2;       /*    CCLR      */
                           unsigned char CKS  :3;       /*    CKS       */
                           }      BIT;                  /*              */
                    }           TCR1;                   /*              */
              union {                                   /* TCSR0        */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char CMFB:1;        /*    CMFB      */
                           unsigned char CMFA:1;        /*    CMFA      */
                           unsigned char OVF :1;        /*    OVF       */
                           unsigned char ADTE:1;        /*    ADTE      */
                           unsigned char OS  :4;        /*    OS        */
                           }      BIT;                  /*              */
                    }           TCSR0;                  /*              */
              union {                                   /* TCSR1        */
                    unsigned char BYTE;                 /*  Byte Access */
                    struct {                            /*  Bit  Access */
                           unsigned char CMFB:1;        /*    CMFB      */
                           unsigned char CMFA:1;        /*    CMFA      */
                           unsigned char OVF :1;        /*    OVF       */
                           unsigned char     :1;        /*              */
                           unsigned char OS  :4;        /*    OS        */
                           }      BIT;                  /*              */
                    }           TCSR1;                  /*              */
              unsigned int      TCORA;                  /* TCORA        */
              unsigned int      TCORB;                  /* TCORB        */
              unsigned int      TCNT;                   /* TCNT         */
};                                                      /*              */
struct st_tmr0 {                                        /* struct TMR0  */
               union {                                  /* TCR          */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char CMIEB:1;      /*    CMIEB     */
                            unsigned char CMIEA:1;      /*    CMIEA     */
                            unsigned char OVIE :1;      /*    OVIE      */
                            unsigned char CCLR :2;      /*    CCLR      */
                            unsigned char CKS  :3;      /*    CKS       */
                            }      BIT;                 /*              */
                     }          TCR;                    /*              */
               char             wk1;                    /*              */
               union {                                  /* TCSR         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char OVF :1;       /*    CMFB      */
                            unsigned char WTIT:1;       /*    WT/IT     */
                            unsigned char TME :1;       /*    TME       */
                            unsigned char     :1;       /*              */
                            unsigned char CKS :4;       /*    CKS       */
                            }      BIT;                 /*              */
                     }          TCSR;                   /*              */
               char             wk2;                    /*              */
               unsigned char    TCORA;                  /* TCORA        */
               char             wk3;                    /*              */
               unsigned char    TCORB;                  /* TCORB        */
               char             wk4;                    /*              */
               unsigned char    TCNT;                   /* TCNT         */
};                                                      /*              */
struct st_tmr1 {                                        /* struct TMR1  */
               union {                                  /* TCR          */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char CMIEB:1;      /*    CMIEB     */
                            unsigned char CMIEA:1;      /*    CMIEA     */
                            unsigned char OVIE :1;      /*    OVIE      */
                            unsigned char CCLR :2;      /*    CCLR      */
                            unsigned char CKS  :3;      /*    CKS       */
                            }      BIT;                 /*              */
                     }          TCR;                    /*              */
               char             wk1;                    /*              */
               union {                                  /* TCSR         */
                     unsigned char BYTE;                /*  Byte Access */
                     struct {                           /*  Bit  Access */
                            unsigned char CMFB:1;       /*    CMFB      */
                            unsigned char CMFA:1;       /*    CMFA      */
                            unsigned char OVF :1;       /*    OVF       */
                            unsigned char     :1;       /*              */
                            unsigned char OS  :4;       /*    OS        */
                            }      BIT;                 /*              */
                     }          TCSR;                   /*              */
               char             wk2;                    /*              */
               unsigned char    TCORA;                  /* TCORA        */
               char             wk3;                    /*              */
               unsigned char    TCORB;                  /* TCORB        */
               char             wk4;                    /*              */
               unsigned char    TCNT;                   /* TCNT         */
};                                                      /*              */
struct st_flash {                                       /* struct FLASH */
                union {                                 /* RAMER        */
                      unsigned char BYTE;               /*  Byte Access */
                      struct {                          /*  Bit  Access */
                             unsigned char wk  :4;      /*              */
                             unsigned char RAMS:1;      /*    RAMS      */
                             unsigned char RAM2:1;      /*    RAM2      */
                             unsigned char RAM1:1;      /*    RAM1      */
                             unsigned char RAM0:1;      /*    RAM0      */
                             }      BIT;                /*              */
                      }         RAMER;                  /*              */
                char            wk[236];                /*              */
                union {                                 /* FLMCR1       */
                      unsigned char BYTE;               /*  Byte Access */
                      struct {                          /*  Bit  Access */
                             unsigned char FWE:1;       /*    FWE       */
                             unsigned char SWE:1;       /*    SWE       */
                             unsigned char ESU:1;       /*    ESU       */
                             unsigned char PSU:1;       /*    PSU       */
                             unsigned char EV :1;       /*    EV        */
                             unsigned char PV :1;       /*    PV        */
                             unsigned char E  :1;       /*    E         */
                             unsigned char P  :1;       /*    P         */
                             }      BIT;                /*              */
                      }         FLMCR1;                 /*              */
                union {                                 /* FLMCR2       */
                      unsigned char BYTE;               /*  Byte Access */
                      struct {                          /*  Bit  Access */
                             unsigned char FLER:1;      /*    FLER      */
                             unsigned char     :5;      /*              */
                             unsigned char ESU :1;      /*     ESU      */
                             unsigned char PSU :1;      /*     PSU      */
                             }      BIT;                /*              */
                      }         FLMCR2;                 /*              */
                union {                                 /* EBR1         */
                      unsigned char BYTE;               /*  Byte Access */
                      struct {                          /*  Bit  Access */
                             unsigned char EB7:1;       /*    EB7       */
                             unsigned char EB6:1;       /*    EB6       */
                             unsigned char EB5:1;       /*    EB5       */
                             unsigned char EB4:1;       /*    EB4       */
                             unsigned char EB3:1;       /*    EB3       */
                             unsigned char EB2:1;       /*    EB2       */
                             unsigned char EB1:1;       /*    EB1       */
                             unsigned char EB0:1;       /*    EB0       */
                             }      BIT;                /*              */
                      }         EBR1;                   /*              */
                union {                                 /* EBR2         */
                      unsigned char BYTE;               /*  Byte Access */
                      struct {                          /*  Bit  Access */
                             unsigned char wk  :4;      /*              */
                             unsigned char EB11:1;      /*    EB11      */
                             unsigned char EB10:1;      /*    EB10      */
                             unsigned char EB9 :1;      /*    EB9       */
                             unsigned char EB8 :1;      /*    EB8       */
                             }      BIT;                /*              */
                      }         EBR2;                   /*              */
};                                                      /*              */
#define P_TPU   (*(volatile struct st_tpu   *)0xFFFFC0) /* TPU   Address*/
#define P_TPU0  (*(volatile struct st_tpu0  *)0xFFFFD0) /* TPU0  Address*/
#define P_TPU1  (*(volatile struct st_tpu1  *)0xFFFFE0) /* TPU1  Address*/
#define P_TPU2  (*(volatile struct st_tpu1  *)0xFFFFF0) /* TPU2  Address*/
#define P_TPU3  (*(volatile struct st_tpu0  *)0xFFFE80) /* TPU3  Address*/
#define P_TPU4  (*(volatile struct st_tpu1  *)0xFFFE90) /* TPU4  Address*/
#define P_TPU5  (*(volatile struct st_tpu1  *)0xFFFEA0) /* TPU5  Address*/
#define P_P1    (*(volatile struct st_p1    *)0xFFFEB0) /* P1    Address*/
#define P_P2    (*(volatile struct st_p1    *)0xFFFEB1) /* P2    Address*/
#define P_P3    (*(volatile struct st_p3    *)0xFFFEB2) /* P3    Address*/
#define P_P4    (*(volatile struct st_p4    *)0xFFFF53) /* P4    Address*/
#define P_PA    (*(volatile struct st_pa    *)0xFFFEB9) /* PA    Address*/
#define P_PB    (*(volatile struct st_pb    *)0xFFFEBA) /* PB    Address*/
#define P_PC    (*(volatile struct st_pb    *)0xFFFEBB) /* PC    Address*/
#define P_PD    (*(volatile struct st_pb    *)0xFFFEBC) /* PD    Address*/
#define P_PE    (*(volatile struct st_pb    *)0xFFFEBD) /* PE    Address*/
#define P_PF    (*(volatile struct st_p1    *)0xFFFEBE) /* PF    Address*/
#define P_PG    (*(volatile struct st_pg    *)0xFFFEBF) /* PG    Address*/
#define P_INTC  (*(volatile struct st_intc  *)0xFFFEC4) /* INTC  Address*/
#define P_BSC   (*(volatile struct st_bsc   *)0xFFFED0) /* BSC   Address*/
#define P_DTC   (*(volatile struct st_dtc   *)0xFFFF30) /* DTC   Address*/
#define P_SBYCR (*(volatile union  un_sbycr *)0xFFFF38) /* SBYCR Address*/
#define P_SYSCR (*(volatile union  un_syscr *)0xFFFF39) /* SYSCR Address*/
#define P_SCKCR (*(volatile union  un_sckcr *)0xFFFF3A) /* SCKCR Address*/
#define P_MDCR  (*(volatile union  un_mdcr  *)0xFFFF3B) /* MDCR  Address*/
#define P_MSTPCR (*(volatile union  un_mstpcr*)0xFFFF3C) /* MSTPCRAddress*/
#define P_SYSCR2 (*(volatile union  un_syscr2*)0xFFFF42) /* SYSCR2Address*/
#define P_SCI0  (*(volatile struct st_sci   *)0xFFFF78) /* SCI0  Address*/
#define P_SCI1  (*(volatile struct st_sci   *)0xFFFF80) /* SCI1  Address*/
#define P_ICI0  (*(volatile struct st_ici   *)0xFFFF78) /* ICI0  Address*/
#define P_ICI1  (*(volatile struct st_ici   *)0xFFFF80) /* ICI1  Address*/
#define P_AD    (*(volatile struct st_ad    *)0xFFFF90) /* A/D   Address*/
#define P_DA    (*(volatile struct st_da    *)0xFFFFA4) /* D/A   Address*/
#define P_TMR   (*(volatile struct st_tmr   *)0xFFFFB0) /* TMR   Address*/
#define P_TMR0  (*(volatile struct st_tmr0  *)0xFFFFB0) /* TMR0  Address*/
#define P_TMR1  (*(volatile struct st_tmr1  *)0xFFFFB1) /* TMR1  Address*/
#define P_FLASH (*(volatile struct st_flash *)0xFFFEDB) /* FLASH Address*/
#define st_tpu3 st_tpu0                         /* Change Struct TPU3   */
#define st_tpu2 st_tpu1                         /* Change Struct TPU2   */
#define st_tpu4 st_tpu1                         /* Change Struct TPU4   */
#define st_tpu5 st_tpu1                         /* Change Struct TPU5   */
#define st_p2   st_p1                           /* Change Struct P2->P1 */
#define st_pf   st_p1                           /* Change Struct PF->P1 */
#define st_pc   st_pb                           /* Change Struct PC->PB */
#define st_pd   st_pb                           /* Change Struct PD->PB */
#define st_pe   st_pb                           /* Change Struct PE->PB */
#endif
