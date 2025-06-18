
#version 330 core	     // Minimal GL version support expected from the GPU

uniform vec3 camPos, lightColor, lightDir;
uniform int is_which;

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoord;
out vec4 color;	  // Shader output: the color response attached to this fragment

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
		texColor = texture(material.backgroundTex, fTexCoord);
		vec3 objectColor = texColor.a == 0.0? texColor.rgb : texColor.rgb / texColor.a;
		color = vec4(objectColor, 1);
		return;
	}
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

	vec3 n = normalize(fNormal);
	float ambientStrength;
	if (is_which == 0){
		ambientStrength = 1;
	}
	else{
		ambientStrength = 0.3;
	}
	
	vec3 ambient = ambientStrength * lightColor;

    float diff = max(dot(n, normalize(lightDir)), 0.0);
    vec3 diffuse = diff * lightColor;
	
	float specularStrength = 0.6;
	vec3 v = normalize(camPos - fPosition); // view vector
	vec3 r = reflect(normalize(-lightDir), n); // relection vector (incident[light2surface], normal) return reflection[surface2light]
	float spec = pow(max(dot(v, r), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;
	vec3 result;

	if (is_which == 0){
		result = ambient * objectColor;	
	}
	else{
		result = (ambient + diffuse + specular) * objectColor;
	}
	color = vec4(result, 1.0); // build an RGBA from an RGB
}
