#define _USE_MATH_DEFINES
#define NOMINMAX

#include "simOpenMesh.h"
#include <simLib/simLib.h>
#include <simLib/scriptFunctionData.h>
#include <simStack/stackArray.h>
#include <simStack/stackMap.h>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <iostream>

typedef OpenMesh::TriMesh_ArrayKernelT<> Mesh;
typedef OpenMesh::Decimater::DecimaterT< Mesh > Decimater;
typedef OpenMesh::Decimater::ModQuadricT< Mesh >::Handle HModQuadric;

static LIBRARY simLib;

bool compute(const double* verticesIn, int verticesInLength, const int* indicesIn, int indicesInLength, double percentageLeft, std::vector<double>& verticesOut, std::vector<int>& indicesOut)
{
        Mesh mesh;
        Decimater decimater(mesh);

        HModQuadric hModQuadric;
        decimater.add(hModQuadric);
        decimater.module(hModQuadric).unset_max_err();

        std::vector<Mesh::VertexHandle> vhandles;
        for (int i = 0; i < verticesInLength / 3; i++)
            vhandles.push_back(mesh.add_vertex(Mesh::Point(verticesIn[3 * i + 0], verticesIn[3 * i + 1], verticesIn[3 * i + 2])));

        std::vector<Mesh::VertexHandle> face_vhandles;
        for (int i = 0; i < indicesInLength / 3; i++)
        {
            face_vhandles.clear();
            face_vhandles.push_back(vhandles[indicesIn[3 * i + 0]]);
            face_vhandles.push_back(vhandles[indicesIn[3 * i + 1]]);
            face_vhandles.push_back(vhandles[indicesIn[3 * i + 2]]);
            mesh.add_face(face_vhandles);
        }

        decimater.initialize();
        decimater.decimate_to_faces(0, int(percentageLeft * double(indicesInLength / 3)));
        mesh.garbage_collection();

        verticesOut.clear();
        Mesh::VertexHandle vh;
        OpenMesh::Vec3f v;
        for (int i = 0; i < int(mesh.n_vertices()); i++)
        {
            vh = Mesh::VertexHandle(i);
            v  = mesh.point(vh);
            verticesOut.push_back(v[0]);
            verticesOut.push_back(v[1]);
            verticesOut.push_back(v[2]);
        }

        indicesOut.clear();
        Mesh::FaceHandle fh;
        OpenMesh::ArrayItems::Face f;
        for (int i = 0; i < int(mesh.n_faces()); i++)
        {
            fh = Mesh::FaceHandle(i);
            mesh.cfv_iter(fh);
            OpenMesh::PolyConnectivity::ConstFaceVertexIter cfv_it=mesh.cfv_iter(fh);
            indicesOut.push_back(cfv_it->idx());
            ++cfv_it;
            indicesOut.push_back(cfv_it->idx());
            ++cfv_it;
            indicesOut.push_back(cfv_it->idx());
        }
        return((verticesOut.size() >= 9) && (indicesOut.size() >= 3));
}

void LUA_DECIMATE_CALLBACK(SScriptCallBack* p)
{
    int stack = p->stackID;

    CStackArray inArguments;
    inArguments.buildFromStack(stack);

    int retVal = -1; // err

    if ( (inArguments.getSize() >= 1) && inArguments.isNumber(0) )
    {
        int shape = inArguments.getInt(0);
        CStackMap* map = nullptr;
        if ( (inArguments.getSize() >= 2) && inArguments.isMap(1) )
            map = inArguments.getMap(1);

        double percentage = 0.8;
        if (map && map->isKeyPresent("percentage"))
            percentage = map->getDouble("percentage");

        double* vert;
        int vertL;
        int* ind;
        int indL;
        int res = simGetShapeMesh(shape, &vert, &vertL, &ind, &indL, nullptr);
        if (res < 0)
            simSetLastError(nullptr, "Invalid shape handle.");
        else
        {
            double m[12];
            simGetObjectMatrix(shape, -1, m);
            for (int i = 0; i < vertL / 3; i++)
                simTransformVector(m, vert + 3 * i);
            std::vector<double> vertices;
            std::vector<int> indices;
            if (compute(vert, vertL, ind, indL, 1.0 - percentage, vertices, indices))
                retVal = simCreateShape(0, 0.0, vertices.data(), vertices.size(), indices.data(), indices.size(), nullptr, nullptr, nullptr, nullptr);
            else
                simSetLastError(nullptr, "Failed decimating shape.");
            simReleaseBuffer(vert);
            simReleaseBuffer(ind);
        }
    }
    else
        simSetLastError(nullptr, "Not enough arguments or wrong arguments.");

    // Return generated shape handle:
    CStackArray outArguments;
    outArguments.pushInt(retVal);
    outArguments.buildOntoStack(stack);
}


// --------------------------------------------------------------------------------------
// simOpenMesh.getDecimated (deprecated)
// --------------------------------------------------------------------------------------
const int inArgs_GETDECIMATED[]={
    4,
    sim_script_arg_double | sim_script_arg_table, 9,
    sim_script_arg_int32 | sim_script_arg_table, 6,
    sim_script_arg_int32, 0,
    sim_script_arg_int32, 0,
};

void LUA_GETDECIMATED_CALLBACK(SScriptCallBack* p)
{ // keep for backward compatibility
    CScriptFunctionData D;
    int result = -1;
    if (D.readDataFromStack(p->stackID, inArgs_GETDECIMATED, inArgs_GETDECIMATED[0], nullptr))
    {
        std::vector<CScriptFunctionDataItem>* inData = D.getInDataPtr();
        double* outV;
        int outVL;
        int* outI;
        int outIL;
        double percent = double(inData->at(3).int32Data[0]) / double(inData->at(1).int32Data.size() / 3);
        int res = simGetDecimatedMesh(&inData->at(0).doubleData[0], inData->at(0).doubleData.size(), &inData->at(1).int32Data[0], inData->at(1).int32Data.size(), &outV, &outVL, &outI, &outIL, percent, 0, NULL);
        if (res > 0)
        {
            std::vector<double> v2(outV, outV + outVL);
            std::vector<int> i2(outI, outI + outIL);
            simReleaseBuffer((char*)outV);
            simReleaseBuffer((char*)outI);
            D.pushOutData(CScriptFunctionDataItem(v2));
            D.pushOutData(CScriptFunctionDataItem(i2));
        }
    }
    D.writeDataToStack(p->stackID);
}
// --------------------------------------------------------------------------------------


SIM_DLLEXPORT int simInit(SSimInit* info)
{
    simLib = loadSimLibrary(info->coppeliaSimLibPath);
    if (simLib == NULL)
    {
        simAddLog(info->pluginName, sim_verbosity_errors, "could not find or correctly load the CoppeliaSim library. Cannot start the plugin.");
        return(0); // Means error, CoppeliaSim will unload this plugin
    }
    if (getSimProcAddresses(simLib) == 0)
    {
        simAddLog(info->pluginName, sim_verbosity_errors, "could not find all required functions in the CoppeliaSim library. Cannot start the plugin.");
        unloadSimLibrary(simLib);
        return(0); // Means error, CoppeliaSim will unload this plugin
    }

    simRegisterScriptCallbackFunction("decimate", nullptr, LUA_DECIMATE_CALLBACK);
    simRegisterScriptCallbackFunction("getDecimated", nullptr, LUA_GETDECIMATED_CALLBACK); // deprecated

    return(6);  // initialization went fine, we return the version number of this extension module (can be queried with simGetModuleName). 5 since V4.6
}

SIM_DLLEXPORT void simCleanup()
{
    unloadSimLibrary(simLib); // release the library
}

SIM_DLLEXPORT void simMsg(SSimMsg*)
{
}

SIM_DLLEXPORT void simDecimateMesh(void* data)
{
    // Collect info from CoppeliaSim:
    void** valPtr = (void**)data;
    double* verticesIn = ((double*)valPtr[0]);
    int verticesInLength = ((int*)valPtr[1])[0];
    int* indicesIn = ((int*)valPtr[2]);
    int indicesInLength = ((int*)valPtr[3])[0];
    double decimationPercentage = ((double*)valPtr[4])[0];
    int interfaceVersion = ((int*)valPtr[5])[0]; // should be zero for the current version

    std::vector<double> verticesOut;
    std::vector<int> indicesOut;
    bool result = compute(verticesIn, verticesInLength, indicesIn, indicesInLength, decimationPercentage, verticesOut, indicesOut);
    ((bool*)valPtr[6])[0] = result;
    if (result)
    {
        double* v = (double*)simCreateBuffer(verticesOut.size() * sizeof(double));
        for (size_t i = 0; i < verticesOut.size(); i++)
            v[i] = verticesOut[i];
        ((double**)valPtr[7])[0] = v;
        ((int*)valPtr[8])[0] = verticesOut.size();
        int* ind = (int*)simCreateBuffer(indicesOut.size() * sizeof(int));
        for (size_t i = 0; i < indicesOut.size(); i++)
            ind[i] = indicesOut[i];
        ((int**)valPtr[9])[0] = ind;
        ((int*)valPtr[10])[0] = indicesOut.size();
    }
}
