#version 330 core

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight { 
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

vec3 CalcDirLight(DirLight light, vec3 camera_dir);

uniform Material material;
uniform DirLight directional_light;
uniform vec3 camera_pos;

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;

void main() {
    // get camera/view direction 
    vec3 camera_dir = normalize(camera_pos - WorldPos);
    // directional light 
    vec3 result = CalcDirLight(directional_light, camera_dir);

    FragColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(DirLight light, vec3 camera_dir) {
    vec3 light_dir = normalize(-light.direction);
    // diffuse component 
    float diff_imp = max(dot(Normal, light_dir), 0.0f);
    // specular component
    vec3 refl_dir = reflect(-light_dir, Normal);
    float spec_imp = pow(max(dot(camera_dir, refl_dir), 0.0f), material.shininess);
    // scale components
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff_imp * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec_imp * vec3(texture(material.specular, TexCoords));
    // combine results
    return (ambient + diffuse + specular);
}