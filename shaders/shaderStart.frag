#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;

in vec3 fpos;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform  vec3 lightColorBulb;
uniform vec3 spotDirection;

uniform vec3 cameraPosEye22;


float cutOff = 0.9f;



// Assuming this is a uniform
//uniform vec3 cameraPosEye; // Assuming this is a uniform

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.7f;
float shininess = 32.0f;
float shadow;

float constant = 1.0f;
float linear = 0.0045f;
float quadratic = 0.0075f;

//for spot lights
uniform int pointOrNot;


uniform vec3 light1;
uniform vec3 light2;
uniform vec3 light3;
uniform vec3 light4;
uniform vec3 light5;
uniform vec3 light6;
uniform vec3 light7;
uniform vec3 light8;
uniform vec3 light9;

uniform vec3 spotlight1;
uniform vec3 spotlight2;

uniform vec3 color1;
uniform vec3 color2;

uniform float fogDensity;


float computeShadow()
{
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    
    if(normalizedCoords.z > 1.0f)
    {
        return 0.0f;
    }
    
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    
    float currentDepth = normalizedCoords.z;
    float bias = 0.005f;
    shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;
    return shadow;
}



void computeLightComponents()
{
    
    
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
    ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
    
    vec3 halfWayVector = normalize(viewDirN + lightDirN);
    
	
	//compute specular light
	vec3 reflection = reflect(-lightDirN, normalEye);
	float specCoeff = pow(max(dot(viewDirN, halfWayVector), 0.0f), shininess);
    specular = specCoeff * lightColor;
    
    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;
    
    //return (ambient + diffuse + specular);
}


vec3 computePointLight(vec3 position, vec3 color)
{
    
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);
    
    //vec3 cameraPosEye = vec3(0.0f);
    //normal
    vec3 normalEye = normalize(fNormal);
    vec3 lightDirN = normalize(position - fpos.xyz);
    vec3 viewDirN = normalize(-fpos.xyz);
    
    //compute distance to light
    float dist = length(position - fpos.xyz);
    //compute attenuation
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));
    
    //compute ambient light
    ambient = att * ambientStrength * color;
    
    //compute diffuse light
    diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * color;
    
    vec3 reflection = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
    
    //compute specular light
    specular = att * specularStrength * specCoeff * color;
    
    ambient *= texture(diffuseTexture, fTexCoords).rgb;
    diffuse *= texture(diffuseTexture, fTexCoords).rgb;
    specular *= texture(specularTexture, fTexCoords).rgb;
    
    
    return (ambient + diffuse + specular);
    
}


vec3 computeSpotLight(vec3 position, vec3 color, vec3 lightDirection)
{
    
    ambient = vec3(0.0);
    diffuse = vec3(0.0);
    specular = vec3(0.0);
    
    
    
    vec3 lightDirN = normalize(position - fpos.xyz);
    float theta = dot(lightDirN, normalize(-lightDirection));
    
    if(theta > cutOff)
    {
        vec3 normalEye = normalize(fNormal);
        vec3 viewDirN = normalize(- fpos.xyz);
        
        //ambient
        ambient = ambientStrength * color;
        
        //diffuse
        diffuse = max(dot(normalEye, lightDirN), 0.0f) * color;
        
        //specular
        vec3 reflection = reflect(-lightDirN, normalEye);
        float specCoeff = pow(max(dot(viewDirN, reflection), 0.0f), shininess);
        specular =  specularStrength * specCoeff * color;
        
        //attenuation
        float distance = length(position - fpos.xyz);
        float att = 1.0f / (constant + linear * distance + quadratic * (distance * distance));
        diffuse *= att;
        specular *= att;
        
        ambient *= texture(diffuseTexture, fTexCoords).rgb;
        diffuse *= texture(diffuseTexture, fTexCoords).rgb;
        specular *= texture(specularTexture, fTexCoords).rgb;
        
        return (ambient + diffuse + specular);
        
    }
    
    return vec3(0.0);
    

}


float computeFog()
{
    //fogDensity = 0.05f;
    float fragmentDistance = length(fPosEye);
    float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

    return clamp(fogFactor, 0.0f, 1.0f);
}




void main() {
    
    
    vec3 color = vec3(0.0); // Initialize final color
    vec3 drrr = normalize(vec3(0.0f, -1.0f, -0.6f)); // At an angle towards the xy-plane
    vec3 drrr2 = normalize(vec3(0.0f, -1.0f, -0.7f));
    
    //drrr.x += 0.5f; // Increase x component
    //drrr.z -= 0.5f; // Decrease z component

    // Normalize the result to keep it as a direction vector
    
    if(pointOrNot == 0)
    {
        computeLightComponents();
        shadow = computeShadow();
        color += min((ambient + (1.0 - shadow) * diffuse) + (1.0 - shadow) * specular, 1.0f);
    }else
    {
        computeLightComponents();
        color += ambient+diffuse+specular;
        //for (int i = 0; i<9; i++) {
            color = color + computePointLight(light1, color1);
            color = color + computePointLight(light2, color1);
            color = color + computePointLight(light3, color1);
            color = color + computePointLight(light4, color1);
            color = color + computePointLight(light5, color1);
            color = color + computePointLight(light6, color1);
            color = color + computePointLight(light7, color1);
            color = color + computePointLight(light8, color1);
            color = color + computePointLight(light9, color1);
            color = color + computeSpotLight(spotlight1, color2,drrr);
            color = color + computeSpotLight(spotlight2, color2,drrr2);

        //}
        //

    }
    
    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
                
    vec3 baseColor = vec3(0.9f, 0.35f, 0.0f); //orange
    
    //fColor = vec4(color, 1.0f); // Ensure the final color is within [0, 1] range
    fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
}
    

