#include "stdafx.h"
#include "object.h"
#include "keycontrol.h"

#define VELOCITY (0.1f)
#define JUMPVELOCITY (20.0f)

const static float MAXSTEP = 1.0f/10000;

extern Object player;
extern KeyControlVector kcv;

static dSpaceID space;
static dWorldID world;
static dGeomID playergeom;
static dBodyID playerbody;
static dJointGroupID contactgroup;
static dJointGroupID persistent_joint_group;
static dJointID player_motor;
static dJointID player_x_slider;
static dJointID player_y_slider;
static int playeronground;
static float dtvar; // dtvar는 dt 값부터 시작해서 0으로 줄어드는 변수

static void
    nearCallback(
    void *data,
    dGeomID o1,
    dGeomID o2
    );

extern float cellw, cellh;
extern int pcx, pcy;
const float hfWidth = cellw*(pcx-1);
const float hfHeight = cellh*(pcy-1);
const int gSuperSamplingRatio = 5;
const int tiles = 2;
const float posxOffset = -tiles/2 * hfWidth;
const float posyOffset = -tiles/2 * hfHeight;

typedef boost::function<void ()> PhyCommand;
tbb::concurrent_queue<PhyCommand> gPhyCommandQueue;

void SetPhyPlayerPosition(float x, float y, float z)
{
    PhyCommand pc = boost::bind(dBodySetPosition, playerbody, x, y, z);
    gPhyCommandQueue.push(pc);
}

static void SetPhyPlayerLinearVelDeltaInternal(float dvx, float dvy, float dvz)
{
    const dReal* curVel = dBodyGetLinearVel(playerbody);
    dBodySetLinearVel(playerbody,
        curVel[0] + dvx,
        curVel[1] + dvy,
        curVel[2] + dvz);
}

static void SetPhyPlayerLinearVelInternal(const float* vx, const float* vy, const float* vz)
{
    int m = 0;

    /*dJointSetLMotorNumAxes(player_motor, m);

    dJointSetLMotorParam(player_motor, dParamVel + 0, 0);
    dJointSetLMotorParam(player_motor, dParamFMax + 0, 0);
    dJointSetLMotorParam(player_motor, dParamVel + 1, 0);
    dJointSetLMotorParam(player_motor, dParamFMax + 1, 0);
    dJointSetLMotorParam(player_motor, dParamVel + 2, 0);
    dJointSetLMotorParam(player_motor, dParamFMax + 2, 0);*/

    if (vx)
    {
        dJointSetSliderParam(player_x_slider, dParamVel, *vx);

        /*dJointSetLMotorParam(player_motor, dParamVel + m, *vx);
        dJointSetLMotorParam(player_motor, dParamFMax + m, *vx ? 1000.0f : 0.0f);

        dJointSetLMotorAxis(player_motor, m, 0, 1, 0, 0);

        ++m;

        dJointSetLMotorNumAxes(player_motor, m);*/
    }

    if (vy)
    {
        dJointSetSliderParam(player_y_slider, dParamVel, *vy);

        /*dJointSetLMotorParam(player_motor, dParamVel + m, *vy);
        dJointSetLMotorParam(player_motor, dParamFMax + m, *vy ? 1000.0f : 0.0f);

        dJointSetLMotorAxis(player_motor, m, 0, 0, 1, 0);

        ++m;

        dJointSetLMotorNumAxes(player_motor, m);*/
    }

    if (vz)
    {
        /*dJointSetLMotorParam(player_motor, dParamVel + m, *vz);
        dJointSetLMotorParam(player_motor, dParamFMax + m, *vz ? 1000.0f : 0.0f);

        dJointSetLMotorAxis(player_motor, m, 0, 0, 0, 1);

        ++m;

        dJointSetLMotorNumAxes(player_motor, m);*/
    }

    /*
    const dReal* curVel = dBodyGetLinearVel(playerbody);
    dBodySetLinearVel(playerbody,
        vx ? *vx : curVel[0],
        vy ? *vy : curVel[1],
        vz ? *vz : curVel[2]);
        */
}

static void SetPhyPlayerLinearVelZeroInternal()
{
    dJointSetLMotorParam(player_motor, dParamVel1, 0);
    dJointSetLMotorParam(player_motor, dParamFMax1, 1000.0f);

    dJointSetLMotorParam(player_motor, dParamVel2, 0);
    dJointSetLMotorParam(player_motor, dParamFMax2, 1000.0f);
}

void SetPhyPlayerLinearVel(const float* vx, const float* vy, const float* vz)
{
    PhyCommand pc = boost::bind(SetPhyPlayerLinearVelInternal, vx, vy, vz);
    gPhyCommandQueue.push(pc);
}

void SetPhyPlayerLinearVelZero()
{
    PhyCommand pc = boost::bind(SetPhyPlayerLinearVelZeroInternal);
    gPhyCommandQueue.push(pc);
}

void SetPhyPlayerLinearVelDelta(float dvx, float dvy, float dvz)
{
    PhyCommand pc = boost::bind(SetPhyPlayerLinearVelDeltaInternal, dvx, dvy, dvz);
    gPhyCommandQueue.push(pc);
}

void SetPhyPlayerForce(float fx, float fy, float fz)
{
    PhyCommand pc = boost::bind(dBodySetForce, playerbody, fx, fy, fz);
    gPhyCommandQueue.push(pc);
}

dReal heightfield_callback( void* pUserData, int x, int z )
{
    extern float cellw, cellh;

    const float posx = posxOffset + cellw/gSuperSamplingRatio * x;
    const float posy = posyOffset + cellh/gSuperSamplingRatio * z;

    float height;
    cl_get_2d_map_query_height(&height, posx, posy);

    return height;
}

int SDLCALL
    PhysicsThread(
    void *data
    )
{
    dInitODE();

    dMass m;
    dVector3 center = {0,0,0,0};
    dVector3 extents = {30,30,30,0};
    //space = dQuadTreeSpaceCreate(0, center, extents, 7);
    space = dHashSpaceCreate(0);
    //space = dSimpleSpaceCreate(0);

    //space = dSweepAndPruneSpaceCreate( 0, dSAP_AXES_XYZ );
    world = dWorldCreate();
    //dWorldSetGravity(world, 0, 0, -50);
    dWorldSetGravity(world, 0, 0, -90.8f);
    //dWorldSetCFM (world, 0.0f);
    //dWorldSetERP (world, 0.5);

    contactgroup = dJointGroupCreate (0);
    persistent_joint_group = dJointGroupCreate (0);


    // Player 설정
    playerbody = dBodyCreate(world);
    //dMassSetSphere(&m, 10, 1);
    dMassSetCapsuleTotal(&m, 1, 2, 1, 4);
    dMassAdjust(&m, 1);
    dBodySetMass(playerbody, &m);
    //dBodySetAngularDamping(playerbody, 0.99999f);
    dBodySetMaxAngularSpeed(playerbody, 0); // rot 끄자~
    //dBodySetKinematic(playerbody);
    

    //playergeom = dCreateSphere(space, 1);
    playergeom = dCreateCapsule(space, 1, 4);
    dGeomSetData(playergeom, "Player");
    dGeomSetBody(playergeom, playerbody);

    dGeomID ray = dCreateRay(space, 50.0f);
    dGeomRaySetParams(ray, FALSE, FALSE);
    
    dMatrix3 rray;
    rray[0*4+0] = 0.0f; rray[0*4+1] = 0.0f; rray[0*4+2] = -1.0f;
    rray[1*4+0] = 0.0f; rray[1*4+1] = 1.0f; rray[1*4+2] = 0.0f;
    rray[2*4+0] = 1.0f; rray[2*4+1] = 0.0f; rray[2*4+2] = 0.0f;
    
    //dGeomSetRotation(ray, rray);
    
    dGeomSetData(ray, "-X Ray");
    dGeomSetBody(ray, playerbody);
    dGeomSetOffsetRotation(ray, rray);
    //dGeomRaySet(ray, 0, 0, 0, -1, 0, 0);

    player_motor = dJointCreateLMotor(world, persistent_joint_group);
    dJointAttach(player_motor, playerbody, 0);
    int n = dJointGetLMotorNumAxes(player_motor);
    dJointSetLMotorNumAxes(player_motor, 2);
    
    dJointSetLMotorParam(player_motor, dParamFMax1, 1000);
    dJointSetLMotorParam(player_motor, dParamFMax2, 1000);
    dJointSetLMotorParam(player_motor, dParamFMax3, 1000);

    /*player_x_slider = dJointCreateSlider(world, persistent_joint_group);
    dJointSetSliderAxis(player_x_slider, 1, 0, 0);
    dJointAttach(player_x_slider, playerbody, 0);*/

    player_y_slider = dJointCreateSlider(world, persistent_joint_group);
    dJointSetSliderAxis(player_y_slider, 0, 1, 0);
    dJointAttach(player_y_slider, playerbody, 0);
    


    dVector3 ax;
    //dJointGetLMotorAxis(player_motor, 0, ax);
    dJointGetLMotorAxis(player_motor, 0, ax);
    dJointGetLMotorAxis(player_motor, 1, ax);
    dJointGetLMotorAxis(player_motor, 2, ax);

    dJointSetLMotorAxis(player_motor, 0, 0, 1, 0, 0);
    dJointSetLMotorAxis(player_motor, 1, 0, 0, 1, 0);
    dJointSetLMotorAxis(player_motor, 2, 0, 0, 0, 1);

    dBodySetPosition(playerbody, 0, 0, 0); // TODO: Physics

    


    dGeomID cylGeom = dCreateCylinder(space, 5, 5);
    //dGeomID cylGeom = dCreateBox(space, 10, 10, 90);
    dGeomSetData(cylGeom, "CollisionOnly");
    dGeomSetPosition(cylGeom, 0, 0, 0);

    // 사방 네 개의 벽
    /*dGeomSetData(dCreatePlane(space, -1, 0, 0, -10), "EastWall");
    dGeomSetData(dCreatePlane(space, 1, 0, 0, -10), "WestWall");
    dGeomSetData(dCreatePlane(space, 0, 1, 0, -10), "SouthWall");
    dGeomSetData(dCreatePlane(space, 0, -1, 0, -10), "NorthWall");*/

    dGeomSetData(dCreatePlane(space, 0, 0, 1, -1), "Ground");

    while (1)
    {
        

        //dSpaceCollide(space, NULL, &nearCallback);
        //dWorldStep(world, MAXSTEP);
        //dJointGroupEmpty(contactgroup);

        const dReal* p = dBodyGetPosition(playerbody);
        const dReal* v = dBodyGetLinearVel(playerbody);

        /*
        player.x = p[0];
        player.y = p[1];
        player.z = p[2];
        player.w = 1.0f;
        */
        memcpy(player.rot, dBodyGetRotation(playerbody), sizeof(player.rot));

        /*
        dBodySetLinearVel(playerbody,
        (kcv.right - kcv.left) * VELOCITY,
        (kcv.up - kcv.down) * VELOCITY,
        ((kcv.jump && playeronground) ? JUMPVELOCITY : v[2]));
        */

        //const dReal* p2 = dGeomGetPosition(cylGeom);
        //printf("%.2f %.2f %.2f\n", p2[0], p2[1], p2[2]);

        //const dReal* p2 = dGeomGetPosition(playergeom);
        //printf("%.2f %.2f %.2f\n", p2[0], p2[1], p2[2]);

        //const dReal* p2 = dBodyGetPosition(playerbody);
        //printf("%.2f %.2f %.2f\n", p2[0], p2[1], p2[2]);

        //cl_player_set_pos(p[0], p[1], p[2]);

        //Sleep(1);

        PhyCommand pc;
        while (gPhyCommandQueue.try_pop(pc))
        {
            pc();
        }
    }

    //dGeomHeightfieldDataDestroy(hf);

    dCloseODE();

    return 0;
}

#define MAX_CONTACTS 64

static void
    nearCallback(
    void *data,
    dGeomID o1,
    dGeomID o2
    )
{
    int i;
    // if (o1->body && o2->body) return;

    // exit without doing anything if the two bodies are connected by a joint
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;

    const char* o1name = (const char*) dGeomGetData (o1);
    const char* o2name = (const char*) dGeomGetData (o2);

    dContact contact[MAX_CONTACTS];   // up to MAX_CONTACTS contacts per box-box
    memset(contact, 0, sizeof(contact));

    bool co = _stricmp(o1name, "CollisionOnly") == 0 || _stricmp(o2name, "CollisionOnly") == 0;

    for (i=0; i<MAX_CONTACTS; i++) {
        contact[i].surface.mode = /*dContactBounce |*/ 0; //dContactSlip1 | dContactSoftCFM;
        contact[i].surface.mu = 10.0f; //co ? 0.0f : 1.0f;
        contact[i].surface.mu2 = 0; //dInfinity;
        contact[i].surface.bounce = 0.0f;
        contact[i].surface.bounce_vel = 0.0f;
        contact[i].surface.soft_cfm = 0.0f;
    }
    if (int numc = dCollide (o1,o2,MAX_CONTACTS,&contact[0].geom,sizeof(dContact))) {
        for (i=0; i<numc; i++) {

            
            if (dGeomGetClass(contact[i].geom.g1) == dRayClass || dGeomGetClass(contact[i].geom.g2) == dRayClass)
            {
                continue;
            }

            if (contact[i].geom.pos[2] > 0.3)
                contact[i].surface.mu = 0;

            dJointID c = dJointCreateContact (world,contactgroup,contact+i);
            dJointAttach (c,b1,b2);
        }
    }
}

static void
    nearCallback2(
    void *data,
    dGeomID o1,
    dGeomID o2
    )
{
    int i, n;

    const char* o1name;
    const char* o2name;
    int o1c, o2c;
    dContact contact[100];

    // exit without doing anything if the two bodies are connected by a joint
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnected(b1, b2))
        return;

    o1name = (const char*) dGeomGetData (o1);
    o2name = (const char*) dGeomGetData (o2);

    // 벽끼리 충돌은 배제
    if (dGeomGetClass(o1) == dPlaneClass && dGeomGetClass(o2) == dPlaneClass)
        return;

    o1c = dGeomGetClass(o1);
    o2c = dGeomGetClass(o2);

    //printf ("collision (%s, %s)\n", o1name, o2name);

    n = dCollide (o1,o2,_countof(contact),&contact[0].geom,sizeof(dContact));

    if ((strcmp(o1name, "Player") == 0 && strcmp(o2name, "Ground") == 0)
        || strcmp(o2name, "Player") == 0 && strcmp(o1name, "Ground") == 0)
    {
        if (n > 0)
            playeronground = 1;
    }

    if (n > 0) {
        for (i=0; i<n; i++) {
            dJointID c;

            contact[i].surface.mode = 0;//dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
            //if (dGeomGetClass(o1) == dSphereClass || dGeomGetClass(o2) == dSphereClass)
            //	contact[i].surface.mu = 20;
            //else
            //	contact[i].surface.mu = 0.5;
            contact[i].surface.mu = 0.5;
            //contact[i].surface.mu = ;
            contact[i].surface.slip1 = 0.0;
            contact[i].surface.slip2 = 0.0;
            contact[i].surface.soft_erp = 0.01f;
            contact[i].surface.soft_cfm = 0.01f;

            c = dJointCreateContact (world,contactgroup,contact+i);
            dJointAttach (c,dGeomGetBody(o1),dGeomGetBody(o2));
        }
    }
}
