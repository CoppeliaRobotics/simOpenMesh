#define _USE_MATH_DEFINES
#define NOMINMAX

#include <iostream>

#include "simOpenMesh.h"
#include <simLib/scriptFunctionData.h>
#include <simLib/simLib.h>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>

typedef OpenMesh::TriMesh_ArrayKernelT<> Mesh;
typedef OpenMesh::Decimater::DecimaterT< Mesh > Decimater;
typedef OpenMesh::Decimater::ModQuadricT< Mesh >::Handle HModQuadric;
// typedef OpenMesh::Decimater::ModRoundnessT< Mesh >::Handle HModRoundness;
// typedef OpenMesh::Decimater::ModHausdorffT< Mesh >::Handle HModHausdorff;
// typedef OpenMesh::Decimater::ModAspectRatioT< Mesh >::Handle HModAspectRatio;
// typedef OpenMesh::Decimater::ModNormalDeviationT< Mesh >::Handle HModNormalDeviation;

#ifdef _WIN32
    #ifdef QT_COMPIL
        #include <direct.h>
        #include <iostream>
    #else
        #include <shlwapi.h>
        #pragma comment(lib, "Shlwapi.lib")
    #endif
#endif /* _WIN32 */
#if defined (__linux) || defined (__APPLE__)
    #include <unistd.h>
#endif /* __linux || __APPLE__ */

#define CONCAT(x,y,z) x y z
#define strConCat(x,y,z)    CONCAT(x,y,z)
static LIBRARY simLib;

bool compute(const double* verticesIn,int verticesInLength,const int* indicesIn,int indicesInLength,double decimationPercentage,std::vector<double>& verticesOut,std::vector<int>& indicesOut)
{
        Mesh mesh;
        Decimater   decimater(mesh);

        HModQuadric hModQuadric;
        decimater.add(hModQuadric);
        decimater.module(hModQuadric).unset_max_err();
    

//      HModRoundness hModRoundness; 
//      decimater.add(hModRoundness);
//      decimater.module(hModRoundness).set_binary(false);

//      HModHausdorff hModHausdorff; 
//      decimater.add(hModHausdorff);
//      decimater.module(hModHausdorff).set_binary(false);

//      HModAspectRatio hModAspectRatio; 
//      decimater.add(hModAspectRatio);
//      decimater.module(hModAspectRatio).set_binary(false);

//      HModNormalDeviation hModNormalDeviation; 
//      decimater.add(hModNormalDeviation);
//      decimater.module(hModNormalDeviation).set_binary(false);


        std::vector<Mesh::VertexHandle> vhandles;
        for (int i=0;i<verticesInLength/3;i++)
            vhandles.push_back(mesh.add_vertex(Mesh::Point(verticesIn[3*i+0],verticesIn[3*i+1],verticesIn[3*i+2])));

        std::vector<Mesh::VertexHandle> face_vhandles;
        for (int i=0;i<indicesInLength/3;i++)
        {
            face_vhandles.clear();
            face_vhandles.push_back(vhandles[indicesIn[3*i+0]]);
            face_vhandles.push_back(vhandles[indicesIn[3*i+1]]);
            face_vhandles.push_back(vhandles[indicesIn[3*i+2]]);
            mesh.add_face(face_vhandles);
        }

        decimater.initialize();
        decimater.decimate_to_faces(0,int(decimationPercentage*double(indicesInLength/3)));
        mesh.garbage_collection();

        verticesOut.clear();
        Mesh::VertexHandle vh;
        OpenMesh::Vec3f v;
        for (int i=0;i<int(mesh.n_vertices());i++)
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
        for (int i=0;i<int(mesh.n_faces());i++)
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
        return((verticesOut.size()>=9)&&(indicesOut.size()>=3));
}


// --------------------------------------------------------------------------------------
// simOpenMesh.getDecimated
// --------------------------------------------------------------------------------------
const int inArgs_DECIMATE[]={
    4,
    sim_script_arg_double|sim_script_arg_table,9,
    sim_script_arg_int32|sim_script_arg_table,6,
    sim_script_arg_int32,0,
    sim_script_arg_int32,0,
};

void LUA_DECIMATE_CALLBACK(SScriptCallBack* p)
{ // keep for backward compatibility
    CScriptFunctionData D;
    int result=-1;
    if (D.readDataFromStack(p->stackID,inArgs_DECIMATE,inArgs_DECIMATE[0],nullptr))
    {
        std::vector<CScriptFunctionDataItem>* inData=D.getInDataPtr();
        double* outV;
        int outVL;
        int* outI;
        int outIL;
        double percent=double(inData->at(3).int32Data[0])/double(inData->at(1).int32Data.size()/3);
        int res=simGetDecimatedMesh(&inData->at(0).doubleData[0],inData->at(0).doubleData.size(),&inData->at(1).int32Data[0],inData->at(1).int32Data.size(),&outV,&outVL,&outI,&outIL,percent,0,NULL);
        if (res>0)
        {
            std::vector<double> v2(outV,outV+outVL);
            std::vector<int> i2(outI,outI+outIL);
            simReleaseBuffer((char*)outV);
            simReleaseBuffer((char*)outI);
            D.pushOutData(CScriptFunctionDataItem(v2));
            D.pushOutData(CScriptFunctionDataItem(i2));
        }
/*
        Mesh mesh;
        Decimater   decimater(mesh);

        HModQuadric hModQuadric;
        decimater.add(hModQuadric);
        decimater.module(hModQuadric).unset_max_err();
    

//      HModRoundness hModRoundness; 
//      decimater.add(hModRoundness);
//      decimater.module(hModRoundness).set_binary(false);

//      HModHausdorff hModHausdorff; 
//      decimater.add(hModHausdorff);
//      decimater.module(hModHausdorff).set_binary(false);

//      HModAspectRatio hModAspectRatio; 
//      decimater.add(hModAspectRatio);
//      decimater.module(hModAspectRatio).set_binary(false);

//      HModNormalDeviation hModNormalDeviation; 
//      decimater.add(hModNormalDeviation);
//      decimater.module(hModNormalDeviation).set_binary(false);


        std::vector<Mesh::VertexHandle> vhandles;
        for (int i=0;i<int(inData->at(0).doubleData.size()/3);i++)
            vhandles.push_back(mesh.add_vertex(Mesh::Point(inData->at(0).doubleData[3*i+0],inData->at(0).doubleData[3*i+1],inData->at(0).doubleData[3*i+2])));

        std::vector<Mesh::VertexHandle> face_vhandles;
        for (int i=0;i<int(inData->at(1).intData.size()/3);i++)
        {
            face_vhandles.clear();
            face_vhandles.push_back(vhandles[inData->at(1).intData[3*i+0]]);
            face_vhandles.push_back(vhandles[inData->at(1).intData[3*i+1]]);
            face_vhandles.push_back(vhandles[inData->at(1).intData[3*i+2]]);
            mesh.add_face(face_vhandles);
        }

        decimater.initialize();
        decimater.decimate_to_faces(inData->at(2).intData[0],inData->at(3).intData[0]);
        mesh.garbage_collection();


        std::vector<double> newVertices;
        Mesh::VertexHandle vh;
        OpenMesh::Vec3f v;
        for (int i=0;i<int(mesh.n_vertices());i++)
        {
            vh = Mesh::VertexHandle(i);
            v  = mesh.point(vh);
            newVertices.push_back(v[0]);
            newVertices.push_back(v[1]);
            newVertices.push_back(v[2]);
        }

        std::vector<int> newIndices;
        Mesh::FaceHandle fh;
        OpenMesh::ArrayItems::Face f;
        for (int i=0;i<int(mesh.n_faces());i++)
        {
            fh = Mesh::FaceHandle(i);
            mesh.cfv_iter(fh);
            OpenMesh::PolyConnectivity::ConstFaceVertexIter cfv_it=mesh.cfv_iter(fh);
            newIndices.push_back(cfv_it->idx());
            ++cfv_it;
            newIndices.push_back(cfv_it->idx());
            ++cfv_it;
            newIndices.push_back(cfv_it->idx());
        }

        D.pushOutData(CScriptFunctionDataItem(newVertices));
        D.pushOutData(CScriptFunctionDataItem(newIndices));
        */
    }
    D.writeDataToStack(p->stackID);
}
// --------------------------------------------------------------------------------------


// This is the plugin start routine:
SIM_DLLEXPORT int simInit(const char* pluginName)
{ // This is called just once, at the start of CoppeliaSim
    // Dynamically load and bind CoppeliaSim functions:
    char curDirAndFile[1024];
#ifdef _WIN32
    #ifdef QT_COMPIL
        _getcwd(curDirAndFile, sizeof(curDirAndFile));
    #else
        GetModuleFileName(NULL,curDirAndFile,1023);
        PathRemoveFileSpec(curDirAndFile);
    #endif
#elif defined (__linux) || defined (__APPLE__)
    getcwd(curDirAndFile, sizeof(curDirAndFile));
#endif

    std::string currentDirAndPath(curDirAndFile);
    std::string temp(currentDirAndPath);

#ifdef _WIN32
    temp+="\\coppeliaSim.dll";
#elif defined (__linux)
    temp+="/libcoppeliaSim.so";
#elif defined (__APPLE__)
    temp+="/libcoppeliaSim.dylib";
#endif /* __linux || __APPLE__ */

    simLib=loadSimLibrary(temp.c_str());
    if (simLib==NULL)
    {
        simAddLog(pluginName,sim_verbosity_errors,"could not find or correctly load the CoppeliaSim library. Cannot start the plugin.");
        return(0); // Means error, CoppeliaSim will unload this plugin
    }
    if (getSimProcAddresses(simLib)==0)
    {
        simAddLog(pluginName,sim_verbosity_errors,"could not find all required functions in the CoppeliaSim library. Cannot start the plugin.");
        unloadSimLibrary(simLib);
        return(0); // Means error, CoppeliaSim will unload this plugin
    }

    // Register the new function:
    simRegisterScriptCallbackFunction("getDecimated",nullptr,LUA_DECIMATE_CALLBACK);

    return(5);  // initialization went fine, we return the version number of this extension module (can be queried with simGetModuleName). 5 since V4.6
}

// This is the plugin end routine:
SIM_DLLEXPORT void simCleanup()
{ // This is called just once, at the end of CoppeliaSim
    unloadSimLibrary(simLib); // release the library
}

// This is the plugin messaging routine (i.e. CoppeliaSim calls this function very often, with various messages):
SIM_DLLEXPORT void simMsg(int,int*,void*)
{
}

SIM_DLLEXPORT void simDecimateMesh(void* data)
{
    // Collect info from CoppeliaSim:
    void** valPtr=(void**)data;
    double* verticesIn=((double*)valPtr[0]);
    int verticesInLength=((int*)valPtr[1])[0];
    int* indicesIn=((int*)valPtr[2]);
    int indicesInLength=((int*)valPtr[3])[0];
    double decimationPercentage=((double*)valPtr[4])[0];
    int interfaceVersion=((int*)valPtr[5])[0]; // should be zero for the current version

    std::vector<double> verticesOut;
    std::vector<int> indicesOut;
    bool result=compute(verticesIn,verticesInLength,indicesIn,indicesInLength,decimationPercentage,verticesOut,indicesOut);
    ((bool*)valPtr[6])[0]=result;
    if (result)
    {
        double* v=(double*)simCreateBuffer(verticesOut.size()*sizeof(double));
        for (size_t i=0;i<verticesOut.size();i++)
            v[i]=verticesOut[i];
        ((double**)valPtr[7])[0]=v;
        ((int*)valPtr[8])[0]=verticesOut.size();
        int* ind=(int*)simCreateBuffer(indicesOut.size()*sizeof(int));
        for (size_t i=0;i<indicesOut.size();i++)
            ind[i]=indicesOut[i];
        ((int**)valPtr[9])[0]=ind;
        ((int*)valPtr[10])[0]=indicesOut.size();
    }
}
