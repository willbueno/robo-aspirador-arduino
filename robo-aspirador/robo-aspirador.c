#include <Ultrasonic.h>
#include <AFMotor.h>
#define TEMPOROT 500 //delay rot 90
#define TEMPOCOMP 500 //delay 180
#define DISTF 8
#define DISTLAT 5
#define TRANS 50
bool rot_hora = true; // True = permite rotacao horario
bool preso = false;
bool inicio_forcado = false;
bool comeco = true;
int cont_rotacao = 0;
float distancia_frente = 100;
float distancia_esquerda = 100;
float distancia_direita = 100;

Ultrasonic sensor_esquerda(24,26);
Ultrasonic sensor_direita(50,48);
Ultrasonic sensor_frente(39, 37);
//esquerda vcc Preta 22, Branco 24, Verde 26, Laranja 28
//direita amarelo-amarelo 46, branco-laranja 48, verde-vermelho 50, vcc amarelo-marrom 52
//frente grd 35, 37, 39, vcc 41

AF_DCMotor motor_esquerda(4); // Motor 1
AF_DCMotor motor_direita(3); // Motor 2

void parar()
{
    motor_direita.run(RELEASE);
    motor_esquerda.run(RELEASE);
    delay(1000);
}

void rotacao_horario()
{
    parar();
    motor_direita.run(FORWARD);
    motor_esquerda.run(BACKWARD);
    Serial.println("Girando 90 graus horario");
    delay(TEMPOROT); // testar o tempo
    parar();
}

void rotacao_anti_horario()
{
    parar();
    motor_direita.run(BACKWARD);
    motor_esquerda.run(FORWARD);
    Serial.println("Girando 90 graus anti horario");
    delay(TEMPOROT); // testar o tempo
    parar();
}

void andar_frente()
{
    motor_direita.run(FORWARD);
    motor_esquerda.run(FORWARD);
    Serial.println("Andando para frente");
}

void andar_tras()
{
    motor_direita.run(BACKWARD);
    motor_esquerda.run(BACKWARD);
    Serial.println("Andando para tras");
}

void rotacao_completa_horario()
{
    rotacao_horario();
    andar_frente();
    delay(TEMPOCOMP); // testar o tempo
    rotacao_horario();
    cont_rotacao++;
}

void rotacao_completa_anti_horario()
{
    rotacao_anti_horario();
    andar_frente();
    delay(TEMPOCOMP); // testar o tempo
    rotacao_anti_horario();
    cont_rotacao++;
}

void setup()
{
    Serial.begin(9600);
    motor_direita.setSpeed(80);
    motor_esquerda.setSpeed(80);
    pinMode(22,OUTPUT);
    digitalWrite(22,HIGH);
    pinMode(28,OUTPUT);
    digitalWrite(28,LOW);
    pinMode(52,OUTPUT);
    digitalWrite(52,HIGH);
    pinMode(46,OUTPUT);
    digitalWrite(46,LOW);
    pinMode(41,OUTPUT);
    digitalWrite(41,HIGH);
    pinMode(35,OUTPUT);
    digitalWrite(35,LOW);
}

void loop()
{
    if(comeco == true)
    {
      comeco = false;
      distancia_frente = 100;
      distancia_direita = 100;
      distancia_esquerda = 100;
      sensor_frente.timing();
    }
    else
    {
    distancia_frente = sensor_frente.timing();
    distancia_frente = distancia_frente / 58;
    distancia_esquerda = sensor_esquerda.timing();
    distancia_esquerda = distancia_esquerda / 58;
    distancia_direita = sensor_direita.timing();
    distancia_direita = distancia_direita / 58;
    }


    if(preso == true)
    {
        andar_tras();
        if(distancia_direita > DISTLAT || distancia_esquerda > DISTLAT)
        {
            inicio_forcado = true;
            preso = false;
        }
    }

    if (preso == false && (distancia_frente < DISTF || inicio_forcado == true))
    {
        inicio_forcado = false;
        if (distancia_direita < DISTLAT && distancia_esquerda < DISTLAT)
        {
            preso = true;
            Serial.println("preso");
        }
        else
        {
            if (rot_hora == true) //True = permite rotacao horario
            {
                if (distancia_direita < DISTLAT)
                {
                    if (cont_rotacao != 0 && cont_rotacao % TRANS == 0) // tranversal
                    {
                        rotacao_anti_horario(); // rotacao 90 graus
                    }
                    else
                    {
                        rotacao_completa_anti_horario(); // rotaciona antihorario
                    }
                }
                else
                {
                    if (cont_rotacao != 0 && cont_rotacao % TRANS == 0)
                    {
                        rotacao_horario();
                    }
                    else
                    {
                        rotacao_completa_horario();
                        rot_hora = false;
                    }
                }
            }//continuar recuo
            else/* if (rot_hora == false)*/ // permite rotacao antihorario
            {
                if (distancia_esquerda < DISTLAT)
                {
                    if (cont_rotacao != 0 && cont_rotacao % TRANS == 0)
                    {
                        rotacao_horario(); // transversal
                    }
                    else
                    {
                        rotacao_completa_horario();
                    }
                }
                else
                {
                    if (cont_rotacao != 0 && cont_rotacao % TRANS == 0)
                    {
                        rotacao_anti_horario(); //transversal
                    }
                    else
                    {
                        rotacao_completa_anti_horario();
                        rot_hora = true;
                    }
                }
            }
        }
    }
    else if (preso == false)
    {
        andar_frente();
    }
}
