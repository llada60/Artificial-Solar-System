#version 330 core           

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal; 
layout(location=2) in vec2 vTexCoord;

uniform mat4 viewMat, projMat, modelMat;
uniform vec3 camPos, lightColor, lightDir;
uniform int is_which;

out vec3 fColor;  // Pass color to the fragment shader
out vec2 fTexCoord;

struct Material{
	sampler2D sunTex;
	sampler2D moonTex;
	sampler2D earthTex; 
	sampler2D venusTex;
	sampler2D marsTex;
	sampler2D jupiterTex;
	sampler2D saturnTex;
	sampler2D uranusTex;
	sampler2D neptuneTex;
	sampler2D mercuryTex;
	sampler2D backgroundTex;
};

uniform Material material;

void main() {
    vec4 texColor;
    if (is_which==-1){
        gl_Position = projMat * viewMat *vec4(vPosition, 1.0);
        texColor = texture(material.backgroundTex, fTexCoord);
        vec3 objectColor = texColor.a == 0.0? texColor.rgb : texColor.rgb / texColor.a;
        fColor = objectColor;
        fTexCoord = vTexCoord;
        return;
    }
    else{
        gl_Position = projMat * viewMat * modelMat * vec4(vPosition, 1.0);
    }
    vec3 normal = normalize(mat3(modelMat) * vNormal);
    vec3 position = vec3(modelMat * vec4(vPosition, 1.0));

    switch(is_which){
		case 0:
			texColor = texture(material.sunTex, fTexCoord);
			break;
		case 1:
			texColor = texture(material.earthTex, fTexCoord);
			break;
		case 2:
			texColor = texture(material.moonTex, fTexCoord);
			break;
		case 3:
			texColor = texture(material.mercuryTex, fTexCoord);
			break;
		case 4:
			texColor = texture(material.venusTex, fTexCoord);
			break;
		case 5:
			texColor = texture(material.marsTex, fTexCoord);
			break;
		case 6:
			texColor = texture(material.jupiterTex, fTexCoord);
			break;
		case 7:
			texColor = texture(material.saturnTex, fTexCoord);
			break;
		case 8:
			texColor = texture(material.uranusTex, fTexCoord);
			break;
		case 9:
			texColor = texture(material.neptuneTex, fTexCoord);
			break;
	}
    vec3 objectColor = texColor.a == 0.0? texColor.rgb : texColor.rgb / texColor.a;
    
    float ambientStrength;
    if (is_which == 0){
        ambientStrength = 1.0;
    }
    else{
        ambientStrength = 0.2;
    }

    vec3 ambient = ambientStrength * objectColor;

    float diff = max(dot(normal, normalize(lightDir)), 0.0);
    vec3 diffuse = diff * lightColor;

    float specularStrength = 0.6;
    vec3 viewDir = normalize(camPos - position);
    vec3 reflectDir = reflect(-normalize(lightDir), normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result;
    if (is_which == 0){
        result = ambient * objectColor;
    }
    else{
        result = (ambient + diffuse + specular) * objectColor;
    }

    // Pass the final color to the fragment shader
    fColor = result;
    fTexCoord = vTexCoord;
}
