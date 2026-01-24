#version 430 core
layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in ;
struct DrawCommand{
	uint count ;
	uint instanceCount ;
	uint firstIndex ;
	uint baseVertex ;
	uint baseInstance ;
};
struct InstanceProperties{
	vec4 position ;
};

layout (std430, binding=1) buffer InstanceData{
	InstanceProperties rawInstanceProps[] ;
};
layout (std430, binding=2) buffer CurrValidInstanceData{
	InstanceProperties currValidInstanceProps[] ;
};
layout (std430, binding=3) buffer DrawCommandsBlock{
	DrawCommand commands[] ;
};

layout(location = 1) uniform mat4 viewMat;
layout(location = 2) uniform mat4 projMat;
layout(location = 3) uniform int numMaxInstance;
layout(location = 4) uniform int grassStartIdx;
layout(location = 5) uniform int bush01StartIdx;
layout(location = 6) uniform int bush05StartIdx;
layout(location = 7) uniform vec4 slimePosition;

void main() {
    const uint idx = gl_GlobalInvocationID.x;
    if(idx >= numMaxInstance){ return; }

    if (rawInstanceProps[idx].position.w == 0.0) { 
        return; 
    }
    float distanceToSlime = length(rawInstanceProps[idx].position.xyz - slimePosition.xyz);
    if (distanceToSlime < 2.0f) { 
        rawInstanceProps[idx].position.w = 0.0;
        return;
    }

    vec4 clipSpaceV = projMat * viewMat * vec4(rawInstanceProps[idx].position.xyz, 1.0);
    clipSpaceV /= clipSpaceV.w;

    bool frustumCulled = (clipSpaceV.x < -1.0) || (clipSpaceV.x > 1.0) ||
                            (clipSpaceV.y < -1.0) || (clipSpaceV.y > 1.0) ||
                            (clipSpaceV.z < -1.0) || (clipSpaceV.z > 1.0);
//    frustumCulled = false;
    if (idx >= grassStartIdx && idx < bush01StartIdx) {
        if (!frustumCulled) {
            const uint UNIQUE_IDX = atomicAdd(commands[0].instanceCount, 1);
            currValidInstanceProps[UNIQUE_IDX] = rawInstanceProps[idx];
        }
    }
    else if (idx >= bush01StartIdx && idx < bush05StartIdx) {
        if (!frustumCulled) {
         const uint UNIQUE_IDX = atomicAdd(commands[1].instanceCount, 1);
         currValidInstanceProps[UNIQUE_IDX + bush01StartIdx] = rawInstanceProps[idx];
        }
    }else{
        if (!frustumCulled) {
            const uint UNIQUE_IDX = atomicAdd(commands[2].instanceCount, 1);
            currValidInstanceProps[UNIQUE_IDX + bush05StartIdx] = rawInstanceProps[idx];
        }
    }
}
