#pragma once

#define VIRUS_STR_1 "Mamba"
#define VIRUS_STR_2 "WannaCry"
#define VIRUS_STR_3 "Petya"

#define ARN_STR_1 "(0)Synthe-18"
#define ARN_STR_2 "(1)GenScript-RX"
#define ARN_STR_3 "(2)Syntho-Alpha"
#define ARN_STR_4 "(3)BioGenic-X5"

#define VIRAL_CODE_STR_1 "(0)SARS-CoV2"
#define VIRAL_CODE_STR_2 "(1)FluX"
#define VIRAL_CODE_STR_3 "(2)RSV-NX"
#define VIRAL_CODE_STR_4 "(3)COVID-X"

#define LIPID_LAYER_STR_1 "(0)Shield-X3"
#define LIPID_LAYER_STR_2 "(1)Core-Alpha"
#define LIPID_LAYER_STR_3 "(2)NanoLip-Lite"
#define LIPID_LAYER_STR_4 "(3)ProteoLip-4"

#define VACCINE_COMP_STR_1 " mRNA "
#define VACCINE_COMP_STR_2 "Codigo Viral"
#define VACCINE_COMP_STR_3 "Capa Lipidica"

typedef enum { VIRUS_1, VIRUS_2, VIRUS_3, VIRUS_NUM } virus_t;

// ARN Mensajero (ARNm) Sintético
typedef enum {
  MRNA_SYNTHEPRO_18,  // mRNA-SynthePro-18
  GENESCRIPT_RX7,     // GeneScript-RX7
  SYNTHORNA_ALPHA,    // SynthoRNA-Alpha
  BIOGENIC_X5         // BioGenic-X5
} mRNA_t;

// Código Genético Viral (Tipos de Virus)
typedef enum {
  VIROGEN_SARS_COV2,  // ViroGen-SARS-CoV2
  VIROGEN_FLUX,       // ViroGen-FluX
  VIROGEN_RSV_NX,     // ViroGen-RSV-NX
  VIROGEN_COVID_X     // ViroGen-COVID-X
} viral_code_t;

// Capa Lipídica (Tipos)
typedef enum {
  LIPOSHIELD_X3,    // LipoShield-X3
  LIPIDCORE_ALPHA,  // LipidCore-Alpha
  NANOLIP_LITE,     // NanoLip-Lite
  PROTEOLIP_4       // ProteoLip-4
} lipid_layer_t;

typedef enum {
  mRNA_COMP,
  VIRAL_CODE_COMP,
  LIPID_LAYER_COMP
} vaccine_components_t;

typedef struct {
  mRNA_t arn;
  viral_code_t viral_code;
  lipid_layer_t lipid_layer;
} vaccine_t;

const char* virus_str[] = {VIRUS_STR_1, VIRUS_STR_2, VIRUS_STR_3};
const char* arn_str[] = {ARN_STR_1, ARN_STR_2, ARN_STR_3, ARN_STR_4};
const char* viral_code_str[] = {VIRAL_CODE_STR_1, VIRAL_CODE_STR_2,
                                VIRAL_CODE_STR_3, VIRAL_CODE_STR_4};
const char* lipid_layer_str[] = {LIPID_LAYER_STR_1, LIPID_LAYER_STR_2,
                                 LIPID_LAYER_STR_3, LIPID_LAYER_STR_4};
const char* vaccine_comp_str[] = {VACCINE_COMP_STR_1, VACCINE_COMP_STR_2,
                                  VACCINE_COMP_STR_3};
const char** components_str[] = {arn_str, viral_code_str, lipid_layer_str};

const vaccine_t cure_1 = {.arn = MRNA_SYNTHEPRO_18,
                          .viral_code = VIROGEN_FLUX,
                          .lipid_layer = NANOLIP_LITE};
const vaccine_t cure_2 = {.arn = GENESCRIPT_RX7,
                          .viral_code = VIROGEN_COVID_X,
                          .lipid_layer = LIPOSHIELD_X3};
const vaccine_t cure_3 = {.arn = SYNTHORNA_ALPHA,
                          .viral_code = VIROGEN_SARS_COV2,
                          .lipid_layer = LIPIDCORE_ALPHA};
const vaccine_t* cures[] = {&cure_1, &cure_2, &cure_3};