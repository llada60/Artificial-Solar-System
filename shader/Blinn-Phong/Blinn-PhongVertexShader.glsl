#version 330 core           

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal; 
layout(location=2) in vec2 vTexCoord;

uniform mat4 viewMat, projMat, modelMat;
uniform int is_which;
out vec3 fPosition;
out vec3 fNormal;
out vec2 fTexCoord;

void main() {
        if (is_which==-1){
                gl_Position = projMat * viewMat *vec4(vPosition, 1.0);
                fPosition = vPosition;
        }
        else{
                gl_Position = projMat * viewMat * modelMat * vec4(vPosition, 1.0);
                fPosition = mat3(transpose(inverse(modelMat))) * vPosition;
        }
        fNormal = normalize(mat3(transpose(inverse(modelMat))) * vNormal); 
        fTexCoord = vTexCoord;
}