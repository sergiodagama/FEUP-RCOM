#ifndef MACROSLD_H
#define MACROSLD_H

/*Formato e tipos de Tramas

    - Tramas de Informação (I):
      | F | A | C | BCC1 | D1 | Data | Dn | BCC2 | F |

      F - Flag
      A - Campo de Endereço
      C - Campo de Controlo -> 0 S 0 0 0 0 0 0  - S = N(s)
      D1, Data, Dn - Campo de Informação (contém pacote gerado pela Aplicação)
      BCC1/2 - Campos de Protecção independentes (1 – cabeçalho, 2 – dados)
             - XOR (^) entre A e C
 
    - Tramas de Supervisão (S) e Não Numeradas (U):
      | F | A | C | BCC1 | F |
 
      F - Flag
      A - Campo de Endereço
      C - Campo de Controlo
          SET -  0 0 0 0 0 0 1 1 (set up)
          DISC - 0 0 0 0 1 0 1 1 (disconnect)
          UA -   0 0 0 0 0 1 1 1 (unnumbered acknowledgment)
          RR -   R 0 0 0 0 1 0 1 (receiver ready / positive ACK)
          REJ -  R 0 0 0 0 0 0 1  - R = N(r) (reject / negative ACK)
      BCC1 - Campo de Protecção (cabeçalho)
*/

#define MAX_ATTEMPS 3
#define ALARM_SECONDS 3
#define MAX_SIZE 200
#define SU_TRAMA_SIZE 5

//Defines of Message content

#define FLAG 0x7E       //01111110 - valor dito nos slides
#define A_EE 0x03       //comandos Enviados pelo Emissor (EE) e Respostas enviadas pelo Recetor
#define A_ER 0x01       //comandos Enviados pelo Recetor (ER) e Respostas enviadas pelo Emissor

//I -> para saber se se recebe I tem de se verificar se é igual
#define C_NS0 0x00
#define C_NS1 0x40

//S ou U

#define C_SET 0x03
#define C_DISC 0x0B
#define C_UA 0x07

#define C_RR_NS0 0x05
#define C_RR_NS1 0x85
#define C_REJ_NS0 0x01
#define C_REJ_NS1 0x81

#define BCC(A, C) A^C

unsigned char SET[SU_TRAMA_SIZE] = {FLAG, A_EE, C_SET, BCC(A_EE, C_SET), FLAG};
unsigned char UA_E[SU_TRAMA_SIZE] = {FLAG, A_EE, C_UA, BCC(A_EE, C_DISC), FLAG};
unsigned char UA_R[SU_TRAMA_SIZE] = {FLAG, A_ER, C_UA, BCC(A_ER, C_UA), FLAG}; 
unsigned char DISC_E[SU_TRAMA_SIZE] = {FLAG, A_EE, C_DISC, BCC(A_EE, C_DISC), FLAG};
unsigned char DISC_R[SU_TRAMA_SIZE] = {FLAG, A_ER, C_DISC, BCC(A_ER, C_DISC), FLAG};

#define FALSE 0
#define TRUE 1

#define ERROR -1
#define ALL_OK 1

/**
 * @brief Connection state
 * 
 */
enum state {CONNECTING, TRANSFERRING, DISCONNECTING, FINISHED};

#endif