xof 0303txt 0032
template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}


Frame Scene_Root {
 

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Frame Star03 {
  

  FrameTransformMatrix {
   589.167297,0.000000,0.000000,0.000000,0.000000,589.167297,0.000000,0.000000,0.000000,0.000000,589.167297,0.000000,-0.104095,-0.073620,0.400296,1.000000;;
  }

  Frame {
   

   FrameTransformMatrix {
    1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,-0.000000,0.000000,-0.000000,1.000000;;
   }

   Mesh {
    6;
    -10.171823;-10.171824;-0.400296;,
    10.171823;-10.171824;-0.400296;,
    -10.171823;10.171824;-0.400296;,
    10.171823;10.171824;-0.400296;,
    10.171823;10.171824;-0.400296;,
    -10.171823;-10.171824;-0.400296;;
    2;
    3;2,0,3;,
    3;1,4,5;;

    MeshNormals {
     6;
     0.000000;0.000000;2.000000;,
     0.000000;0.000000;1.000000;,
     0.000000;0.000000;1.000000;,
     0.000000;0.000000;2.000000;,
     0.000000;0.000000;2.000000;,
     0.000000;0.000000;2.000000;;
     2;
     3;2,0,3;,
     3;1,4,5;;
    }

    MeshTextureCoords {
     6;
     0.000000;1.000000;,
     1.000000;1.000000;,
     0.000000;0.000000;,
     1.000000;0.000000;,
     1.000000;0.000000;,
     0.000000;1.000000;;
    }

    VertexDuplicationIndices {
     6;
     4;
     0,
     1,
     2,
     3,
     3,
     0;
    }

    MeshMaterialList {
     1;
     2;
     0,
     0;

     Material {
      1.000000;1.000000;1.000000;1.000000;;
      0.000000;
      1.000000;1.000000;1.000000;;
      0.000000;0.000000;0.000000;;

      TextureFilename {
       "C:\\Temp\\zspace\\earth.bmp";
      }
     }
    }
   }
  }
 }
}