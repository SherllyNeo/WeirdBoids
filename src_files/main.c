#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>


#define SCREENWIDTH  1000
#define SCREENHEIGHT 1000
#define AMOUNT 100
#define TIPLEN 25
#define WINGSPAN 15
#define BACKLEN 10
#define RADIUSOFVIEW (TIPLEN+BACKLEN)*5
#define ANGLEOFVIEW PI
#define FRICTIOM 30
#define COLLISONDISTANCE TIPLEN*5


int steerCount = 0;
int steerCountNorm = 0;



/*

   Boids 

   1. Give them an angle of view
   2. Make them avoid boids in their angle of view Seperation
   3. Make them follow the average direction of boids in their view Alignment 
   4. steer towards center of their flock Cohesion


*/



typedef struct Boid {
    float Xpos;
    float Ypos;
    float speed;
    float rotationalSpeed;
    float direction;
    Color Colour;
} Boid;

bool randomBool()
{
    return 0 + (rand() % (1 - 0 + 1)) == 1;
}

Boid InitBoid() {
    int Xpos = rand() % (SCREENWIDTH - (BACKLEN+TIPLEN));
    int Ypos = rand() % (SCREENHEIGHT - (BACKLEN+TIPLEN));
    float speed = 0.2;
    float rotationalSpeed = 0.04;
    float direction = rand() % (int)(2*PI);
    Color colour = (randomBool() ? LIGHTGRAY : BLUE);
    Boid boid = (Boid){Xpos,Ypos,speed,rotationalSpeed,direction,colour};
    return boid;
}


Vector2 rotateVectorAroundCenter(Vector2 vec, float direction, int Cx, int Cy) {
    Vector2 result;

    float OgVecx = vec.x;
    float OgVecy = vec.y;

    vec.x -= Cx;
    vec.y -= Cy;

    result.x = vec.x * (float)cos(direction) - vec.y * (float)sin(direction);
    result.y = vec.x * (float)sin(direction) + vec.y * (float)cos(direction);

    result.x += Cx;
    result.y += Cy;


    return result;
}

void DrawBoid(Boid* b) {
    /* 

       We take the points of the triangle, Tip, Wing1 and Wing2.
       Which we define from the coordinate center 
       C = (Cx,Cy)

       Then we define the points of the triangle, imagining it horziontally aligned on the X axis, we want them to be 10 long

       TipX = Cx + TIPLEN
       TipY = Cy

       The wing2 is the same as wing 1 but mirroed in the line Y=Cy

       Wing1X = Cx - BACKLEN
       Wing2Y = Cy + WINGSPAN

       Wing2X = Cx - BACKLEN
       Wing2Y = Cy - WINGSPAN

       apply the rotation matrix for all of these values (make sure to send to origin, rotate then transelate back)
       x = cos(theta)x - sin(theta)y
       y = cos(theta)y + sin(theta)x

*/ 


    Vector2 TipFlat = {b->Xpos + TIPLEN,b->Ypos};
    Vector2 Wing1Flat = {b->Xpos - BACKLEN,b->Ypos - WINGSPAN};
    Vector2 Wing2Flat = {b->Xpos - BACKLEN,b->Ypos + WINGSPAN};

    Vector2 Tip = rotateVectorAroundCenter(TipFlat, b->direction,b->Xpos,b->Ypos);
    Vector2 Wing1 = rotateVectorAroundCenter(Wing1Flat, b->direction,b->Xpos,b->Ypos);
    Vector2 Wing2 = rotateVectorAroundCenter(Wing2Flat, b->direction,b->Xpos,b->Ypos);

    DrawTriangle(Tip, Wing1, Wing2, b->Colour);
}

void UpdateMovement(Boid* currentBoid,float averagedDirection,Vector2 averagedCenter,Vector2 NearestBoidCenter,float NearestBoidDistance) { 

    float ChangeInDir = 0;

    Vector2 mousePoint = { 0.0f, 0.0f };

    mousePoint = GetMousePosition();

    //direction to nearestBoid
    float angleToNearestBoid = 0.0;
    if (NearestBoidDistance > 0.0) {
         angleToNearestBoid = (atan((currentBoid->Ypos - NearestBoidCenter.x)/(currentBoid->Xpos - NearestBoidCenter.x)));
    }
    
    //direction to mouse
    float angleToMouse = (atan((currentBoid->Ypos - mousePoint.y)/(currentBoid->Xpos - mousePoint.x)));

    //direction to average Center
    float dirToCenter = (atan((currentBoid->Ypos - averagedCenter.y)/(currentBoid->Xpos - averagedCenter.x)));
    if (averagedCenter.x <= currentBoid->Xpos) {
        dirToCenter = dirToCenter+PI;
    }


    //currentBoid->direction += currentBoid->rotationalSpeed*(averagedDirection + angleToCenter - angleToBoid);

    if ((NearestBoidDistance < COLLISONDISTANCE) || steerCount > 0) {
        if (steerCount > 0) {
            steerCount--;
        }
        else {
            steerCount = 30;
        }
        if (NearestBoidCenter.x <= currentBoid->Xpos) {
            angleToNearestBoid = angleToNearestBoid+PI;
        }
      ChangeInDir = -currentBoid->rotationalSpeed*angleToNearestBoid;
    }
    else {
        if (mousePoint.x <= currentBoid->Xpos) {
            angleToMouse = angleToMouse+PI;
        }
        ChangeInDir = currentBoid->rotationalSpeed*(angleToMouse+dirToCenter+averagedDirection);
    }
    /*
     Push along direction
     */
    currentBoid->Xpos += (currentBoid->speed)*cos(currentBoid->direction);
    currentBoid->Ypos += (currentBoid->speed)*sin(currentBoid->direction);

    if (currentBoid->Xpos > SCREENWIDTH || currentBoid->Xpos < 0) {
        currentBoid->Xpos = (float)(((int)currentBoid->Xpos+SCREENWIDTH) % SCREENWIDTH);
    }
    if (currentBoid->Ypos > SCREENHEIGHT || currentBoid->Ypos < 0) {
        currentBoid->Ypos = (float)(((int)currentBoid->Ypos+SCREENHEIGHT) % SCREENHEIGHT);
    }


    if (steerCountNorm <= 0) {
        if (ChangeInDir < 0.0) {
            currentBoid->direction -= currentBoid->rotationalSpeed;
        }
        else {
            currentBoid->direction += currentBoid->rotationalSpeed;
        }
        steerCountNorm = 10;
    }
    steerCountNorm--;
    


}


int main(int argc, char *argv[])
{
    srand(time(NULL));

    SetTargetFPS(25);
    InitWindow(SCREENWIDTH, SCREENHEIGHT,"Boids");
    Boid *Boids = malloc(sizeof(Boid)*AMOUNT);
    int boids = AMOUNT;
    for (int a = 0; a<boids;a++) {
        Boid newBoid = InitBoid();
        Boids[a] = newBoid;
    }




    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        BeginDrawing();
        ClearBackground(BROWN);

        for (int i=0;i<boids;i++) {
            float averagedDirection = 0.0;
            Vector2 averagedCenter = {0.0,0.0};

            Vector2 NearestBoidCenter = {0.0,0.0};
            float NearestBoidDistance = SCREENWIDTH*SCREENHEIGHT*1000;

            int BoidsInView = 0;


            Boid* currentBoid = &Boids[i];
            for (int j=0;j<boids;j++) {
                if (j != i) {
                    Boid* currentBoidConsidered = &Boids[j];
                    float dx = currentBoidConsidered->Xpos - currentBoid->Xpos;
                    float dy = currentBoidConsidered->Ypos - currentBoid->Ypos;
                    float DistanceToOurBoid = (float)sqrt((double)(pow(dx,2)+pow(dy,2)));
                    float AngleToOurBoid = 0.0; // Add in angle check later
                    if (DistanceToOurBoid < NearestBoidDistance) {
                        NearestBoidDistance = DistanceToOurBoid;
                        NearestBoidCenter.x = currentBoidConsidered->Xpos;
                        NearestBoidCenter.y = currentBoidConsidered->Ypos;
                    }
                    if (DistanceToOurBoid <= RADIUSOFVIEW) {
                        BoidsInView++;
                        /* currentBoid can see currentBoidConsidered so we need to take into account the rules */
                        averagedDirection += currentBoidConsidered->direction / (float)BoidsInView;
                        averagedCenter.x += currentBoidConsidered->Xpos / (float)BoidsInView;
                        averagedCenter.y += currentBoidConsidered->Ypos / (float)BoidsInView;

                        }
                }
                averagedDirection = averagedDirection;
                averagedCenter.x = averagedCenter.x;
                averagedCenter.y = averagedCenter.y;

                if (i == 0) {
                    currentBoid->Colour = RED;
                    //printf("boid close to (%f,%f), direction: %f, speed: %f\n",currentBoid->Xpos,currentBoid->Ypos,currentBoid->direction,currentBoid->speed);
                }
                UpdateMovement(currentBoid,averagedDirection,averagedCenter,NearestBoidCenter,NearestBoidDistance);
                DrawBoid(currentBoid);
            }
        }


        EndDrawing();

    }
    CloseWindow();        // Close window and OpenGL context
    return EXIT_SUCCESS;
}
