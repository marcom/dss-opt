#ifndef DSSOPT_HEADER_NN_PARAM_H
#define DSSOPT_HEADER_NN_PARAM_H


#define NN_INF 1000000
#define NN_LXC37 107.856
#define NN_N_HAIRPINLOOP 31
#define NN_N_BULGELOOP 31
#define NN_N_INTERNALLOOP 31
#define NN_ML_OFFSET 340
#define NN_ML_UNPAIRED 0
#define NN_ML_STEMS 40
#define NN_G_NON_GC_PENALTY 50
#define NN_NINIO_M 50
#define NN_NINIO_MAX 300
#define NN_NTETRALOOP 30

extern const int nn_G_stack[6][6];
extern const int nn_G_mismatch_hairpin[6][4][4];
extern const int nn_G_mismatch_interior[6][4][4];
extern const int nn_G_dangle5[6][4];
extern const int nn_G_dangle3[6][4];
extern const int nn_G_int11[6][6][4][4];
extern const int nn_G_int21[6][6][4][4][4];
extern const int nn_G_int22[6][6][4][4][4][4];
extern const int nn_G_hairpin_loop[31];
extern const int nn_G_bulge_loop[31];
extern const int nn_G_internal_loop[31];
extern const int nn_G_ml_param[4];
extern const int nn_G_ninio[2];
extern const unsigned int nn_tetraloop_bp[];
extern const unsigned int nn_tetraloop_loop[][4];
extern const int nn_G_tetraloop[];


#endif /* DSSOPT_HEADER_NN_PARAM_H */
