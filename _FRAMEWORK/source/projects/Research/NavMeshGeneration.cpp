#include "stdafx.h"
#include "NavMeshGeneration.h"
#include "NavMesh.h"


NavMesh* NavMeshGeneration::CreateNavMesh(Geometry::Polygon& bounds, std::initializer_list<Geometry::Polygon> obstacles, float playerRadius)
{
    NavMesh* pNavMesh = new NavMesh{};
    Triangulate(pNavMesh, bounds);
    return pNavMesh;
}


void NavMeshGeneration::Triangulate(NavMesh* pNavMesh, Geometry::Polygon& bounds)
{
    Geometry::VertexNode* v0, * v1, * v2, * v3, * v4;

    uint64_t n = bounds.size; //TODO: change to nvertices -> is this 5 or is this the number of vertices in the polygon

    EarInit(bounds);

    while (n > 3)
    {
        v2 = bounds.pHead;
        do
        {
            if (v2->ear)
            {
                v3 = v2->pNext;
                v4 = v3->pNext;
                v1 = v2->pPrevious;
                v0 = v1->pPrevious;

                std::cout << "Diagonal: " << std::to_string(v1->index) << ", " << std::to_string(v3->index) << std::endl;

                v1->ear = Diagonal(v0, v3, bounds);
                v3->ear = Diagonal(v1, v4, bounds);

                //Add to nav mesh
                pNavMesh->AddTriangle(bounds.pHead, bounds.pHead->pNext, bounds.pHead->pPrevious);

                v1->pNext = v3;
                v3->pPrevious = v1;
                SAFE_DELETE(bounds.pHead);
                bounds.pHead = v3;
                --bounds.size;
                --bounds.nextIndex;
                --n;
                break;
            }

            v2 = v2->pNext;
        } while (v2 != bounds.pHead);
    }

    pNavMesh->AddTriangle(bounds.pHead, bounds.pHead->pNext, bounds.pHead->pPrevious);
    SAFE_DELETE(bounds.pHead->pNext);
    SAFE_DELETE(bounds.pHead->pPrevious);
    SAFE_DELETE(bounds.pHead);
}

void NavMeshGeneration::EarInit(Geometry::Polygon& bounds)
{
    Geometry::VertexNode* v0, * v1, * v2;

    v1 = bounds.pHead;
    do
    {
        v2 = v1->pNext;
        v0 = v1->pPrevious;
        v1->ear = Diagonal(v0, v2, bounds);
        v1 = v1->pNext;
    } while (v1 != bounds.pHead);
}

bool NavMeshGeneration::Diagonal(Geometry::VertexNode* v0, Geometry::VertexNode* v1, Geometry::Polygon& bounds)
{
    return InCone(v0, v1) && InCone(v1, v0) && Diagonalie(v0, v1, bounds);
}

bool NavMeshGeneration::InCone(Geometry::VertexNode* v0, Geometry::VertexNode* v1)
{
    //Geometry::VertexNode* v2, * v3;

    //v2 = v0->pPrevious;
    //v3 = v0->pNext;

    //Elite::Vector2 v0pos{ v0->vertex.position.x, v0->vertex.position.y };
    //Elite::Vector2 v1pos{ v1->vertex.position.x, v1->vertex.position.y };
    //Elite::Vector2 v2pos{ v2->vertex.position.x, v2->vertex.position.y };
    //Elite::Vector2 v3pos{ v3->vertex.position.x, v3->vertex.position.y };

    //if (LeftOn(v0pos, v3pos, v2pos)) //vertex is convex
    //    return Left(v0pos, v1pos, v2pos) && Left(v1pos, v0pos, v3pos);

    ////vertex is reflex
    //return !(LeftOn(v0pos, v1pos, v3pos) && LeftOn(v1pos, v0pos, v2pos));
    return true;
}

bool NavMeshGeneration::Diagonalie(Geometry::VertexNode* v0, Geometry::VertexNode* v1, Geometry::Polygon& bounds)
{
    /*Geometry::VertexNode* v2, * v3;

    v2 = bounds.pHead;

    do
    {
        v3 = v2->pNext;

        Elite::Vector2 p0{ v0->vertex.position.x, v0->vertex.position.y };
        Elite::Vector2 p1{ v1->vertex.position.x, v1->vertex.position.y };
        Elite::Vector2 p2{ v2->vertex.position.x, v2->vertex.position.y };
        Elite::Vector2 p3{ v3->vertex.position.x, v3->vertex.position.y };

        if ((v2 != v0) && (v3 != v0) && (v2 != v1) && (v3 != v1)
            && Intersect(p0, p1, p2, p3))
            return false;

        v2 = v2->pNext;

    } while (v2 != bounds.pHead);*/

    return true;
}

bool NavMeshGeneration::Left(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2)
{
    return DoubleArea(p0, p1, p2) > 0;
}

bool NavMeshGeneration::LeftOn(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2)
{
    return DoubleArea(p0, p1, p2) >= 0;
}

bool NavMeshGeneration::Collinear(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2)
{
    return DoubleArea(p0, p1, p2) == 0;
}

bool NavMeshGeneration::Intersect(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2, Elite::Vector2& p3)
{
    if (IntersectProp(p0, p1, p2, p3))
        return true;
    else if (Between(p0, p1, p2) || Between(p0, p1, p3) || Between(p2, p3, p0) || Between(p2, p3, p1))
        return true;
    else
        return false;
}

bool NavMeshGeneration::IntersectProp(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2, Elite::Vector2& p3)
{
    if (Collinear(p0, p1, p2) || Collinear(p0, p1, p3) || Collinear(p2, p3, p0) || Collinear(p2, p3, p1))
        return false;

    return Xor(Left(p0,p1,p2), Left(p0, p1, p3)) && Xor(Left(p2, p3, p0), Left(p2, p3, p1));
}

int NavMeshGeneration::DoubleArea(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2)
{
    return (int(p1.x) - int(p0.x)) * (int(p2.y) - int(p0.y)) - (int(p2.x) - int(p0.x)) * (int(p1.y) - int(p0.y));
}

bool NavMeshGeneration::Between(Elite::Vector2& p0, Elite::Vector2& p1, Elite::Vector2& p2)
{
    if (!Collinear(p0, p1, p2))
        return false;

    if (p0.x != p1.x)
        return (p0.x <= p2.x && p2.x <= p1.x) || (p0.x >= p2.x && p2.x >= p1.x);
    else
        return (p0.y <= p2.y && p2.y <= p1.y) || (p0.y >= p2.y && p2.y >= p1.y);
}
