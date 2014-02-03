#include "stdafx.h"
#include "component_collision.h"
#include "logicworld.h"
#include "ClContext.h"
#include "component_pos0.h"
#include "clientlogic_private.h"

using namespace component;

inline ClContext* ClCtx();

collision::collision()
    : type(collision::cylinder), x(3.0), y(3.0), z(5.0)
{
}

collision::~collision()
{
}

void collision::add_to_world(client::entity e)
{
    btVector3 pos(C<pos0>(e).x, C<pos0>(e).y, C<pos0>(e).z);
    //ClCtx()->lw->AddCylinderShape(pos, btVector3(x, y, z));

    btCylinderShapeZ* shape = new btCylinderShapeZ(btVector3(3,3,5));
    bt_obj.setCollisionShape(shape);
    
    btTransform startTransform;
    startTransform.setIdentity();
    startTransform.setOrigin(pos);

    bt_obj.setWorldTransform(startTransform);
    ClCtx()->lw->AddCollisionObject(&bt_obj);
}

void collision::remove_from_world()
{
    ClCtx()->lw->RemoveCollisionObject(&bt_obj);
}
