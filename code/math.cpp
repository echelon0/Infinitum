 
#include <math.h>

#define X_AXIS 1
#define Y_AXIS 2
#define Z_AXIS 3

#define PI 3.14159

struct int2 {
    int x;
    int y;

    int2() {
        this->x = 0;
        this->y = 0;
    }
    
    int2(int x, int y) {
        this->x = x;
        this->y = y;
    }

    bool operator == (int2 rhs) {
        return (this->x == rhs.x) && (this->y == rhs.y);
    }

    int2 operator / (int scalar) {
        return int2(this->x / scalar, this->y / scalar);
    }
    int2 operator / (int2 rhs) {
        return int2(this->x / rhs.x, this->y / rhs.y);
    }
};


struct float2 {
    float x;
    float y;
    
    float2() {
        this->x = 0.0f;
        this->y = 0.0f;
    }
    
    float2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    bool operator == (float2 rhs) {
        return (this->x == rhs.x) && (this->y == rhs.y);
    }

    bool operator > (float2 rhs) {
        return sqrt(this->x * this->x + this->y * this->y) > sqrt(rhs.x * rhs.x + rhs.y * rhs.y);
    }
};

struct float3 {
    float x;
    float y;
    float z;

    float3() {
        this->x = 0.0f;
        this->y = 0.0f;
        this->z = 0.0f;
    }
    
    float3(float x, float y, float z) {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    
    float3& operator += (const float3 &rhs) {
        this->x += rhs.x;
        this->y += rhs.y;
        this->z += rhs.z;
        return *this;
    }

    float3& operator -= (const float3 &rhs) {
        this->x -= rhs.x;
        this->y -= rhs.y;
        this->z -= rhs.z;
        return *this;
    }

    bool operator == (float3 rhs) {
        return (this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z);
    }

    bool operator != (float3 rhs) {
        return !(*this == rhs);
    }
};

struct float4 {
    float x;
    float y;
    float z;
    float w;
    
    float4() {
        this->x = 0.0f;
        this->y = 0.0f;
        this->z = 0.0f;
        this->w = 0.0f;
    }
    
    float4(float x, float y, float z, float w) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    float4(float3 vec, float w) {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        this->w = w;    
    }
    bool operator == (float4 rhs) {
        return (this->x == rhs.x) && (this->y == rhs.y) && (this->z == rhs.z) && (this->w == rhs.w);
    }
};

struct float3x3 {
    float data[3][3];
    
    float3x3() {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                this->data[i][j] = 0.0f;
            }
        }
    }
    
    float3x3(float entry_11, float entry_12, float entry_13,
          float entry_21, float entry_22, float entry_23,
          float entry_31, float entry_32, float entry_33) {

        this->data[0][0] = entry_11;
        this->data[0][1] = entry_12;
        this->data[0][2] = entry_13;

        this->data[1][0] = entry_21;
        this->data[1][1] = entry_22;
        this->data[1][2] = entry_23;
        
        this->data[2][0] = entry_31;
        this->data[2][1] = entry_32;
        this->data[2][2] = entry_33;
    }
    
    float3x3 operator = (float3x3 rhs) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                this->data[i][j] = rhs.data[i][j];
            }
        }
        return *this;
    }

    bool operator == (float3x3 rhs) {
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                if(this->data[i][j] != rhs.data[i][j])
                    return false;
            }
        }
        return true;
    }
};

struct float4x4 {
    float data[4][4];
    
    float4x4() {
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                this->data[i][j] = 0.0f;
            }
        }
    }
    
    float4x4(float entry_11, float entry_12, float entry_13, float entry_14,
          float entry_21, float entry_22, float entry_23, float entry_24,
          float entry_31, float entry_32, float entry_33, float entry_34,
          float entry_41, float entry_42, float entry_43, float entry_44) {

        this->data[0][0] = entry_11;
        this->data[0][1] = entry_12;
        this->data[0][2] = entry_13;
        this->data[0][3] = entry_14;

        this->data[1][0] = entry_21;
        this->data[1][1] = entry_22;
        this->data[1][2] = entry_23;
        this->data[1][3] = entry_24;
        
        this->data[2][0] = entry_31;
        this->data[2][1] = entry_32;
        this->data[2][2] = entry_33;
        this->data[2][3] = entry_34;
        
        this->data[3][0] = entry_41;
        this->data[3][1] = entry_42;
        this->data[3][2] = entry_43;
        this->data[3][3] = entry_44;
    }
    
    float4x4 operator = (float4x4 rhs) {
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                this->data[i][j] = rhs.data[i][j];
            }
        }
        return *this;
    }

    bool operator == (float4x4 rhs) {
        for(int i = 0; i < 4; i++) {
            for(int j = 0; j < 4; j++) {
                if(this->data[i][j] != rhs.data[i][j])
                    return false;
            }
        }
        return true;
    }
};

struct quat {
    float x;
    float y;
    float z;
    float w;

    quat() {
        this->x = 0.0f;
        this->y = 0.0f;
        this->z = 0.0f;
        this->w = 1.0f;
    }

    bool operator == (quat rhs) {
        if(this->x != rhs.x ||
           this->y != rhs.y ||
           this->z != rhs.z ||
           this->w != rhs.w) {
            return false;
        }
        return true;
    }
};

inline float
sign(float t) {
    return (t < 0.0f)? -1.0f : 1.0f;
}

inline int2
operator - (int2 lhs, int2 rhs) {
    int2 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    return result;
}

inline float2
operator * (float2 vector, float scaler) {
    float2 result;
    result.x = vector.x * scaler;
    result.y = vector.y * scaler;
    return result;
}

inline float2
operator * (float scaler, float2 vector) {
    float2 result;
    result.x = vector.x * scaler;
    result.y = vector.y * scaler;
    return result;
}

inline float2
operator / (float2 vector, float scaler) {
    float2 result;
    result.x = vector.x / scaler;
    result.y = vector.y / scaler;
    return result;    
}

inline float2
operator + (float2 lhs, float2 rhs) {
    float2 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    return result;    
}

inline float2
operator - (float2 lhs, float2 rhs) {
    float2 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    return result;    
}

inline float
magnitude(float2 source) {
    return sqrtf(source.x * source.x + source.y * source.y);
}

inline float2
normalize(float2 source) {
    if(source == float2())
        return source;
    
    float2 result;
    float source_length = magnitude(source);
    float multiplier = 1.0f / source_length;
    
    result.x = source.x * multiplier;
    result.y = source.y * multiplier;
    
    return result;
}

inline float3
operator * (float3 vector, float scaler) {
    float3 result;
    result.x = vector.x * scaler;
    result.y = vector.y * scaler;
    result.z = vector.z * scaler;
    return result;
}

inline float3
operator * (float scaler, float3 vector) {
    float3 result;
    result.x = vector.x * scaler;
    result.y = vector.y * scaler;
    result.z = vector.z * scaler;
    return result;
}

inline float3
operator / (float3 vector, float scaler) {
    float3 result;
    result.x = vector.x / scaler;
    result.y = vector.y / scaler;
    result.z = vector.z / scaler;
    return result;    
}

inline float3
operator + (float3 lhs, float3 rhs) {
    float3 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;    
}

inline float3
operator - (float3 lhs, float3 rhs) {
    float3 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;    
}

inline float4
operator * (float4 vector, float scaler) {
    float4 result;
    result.x = vector.x * scaler;
    result.y = vector.y * scaler;
    result.z = vector.z * scaler;
    result.w = vector.w * scaler;
    return result;
}

inline float4
operator * (float scaler, float4 vector) {
    float4 result;
    result.x = vector.x * scaler;
    result.y = vector.y * scaler;
    result.z = vector.z * scaler;
    result.w = vector.w * scaler;
    return result;
}

inline float4
operator / (float4 vector, float scaler) {
    float4 result;
    result.x = vector.x / scaler;
    result.y = vector.y / scaler;
    result.z = vector.z / scaler;
    result.w = vector.w / scaler;
    return result;    
}

inline float4
operator + (float4 lhs, float4 rhs) {
    float4 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    result.w = lhs.w + rhs.w;
    return result;    
}

inline float4
operator - (float4 lhs, float4 rhs) {
    float4 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    result.w = lhs.w - rhs.w;
    return result;    
}

inline float4x4
operator * (float4x4 lhs, float4x4 rhs) {
    float4x4 result;
    float ElementValue;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            ElementValue = 0;
            for(int k = 0; k < 4; ++k) {
                ElementValue += (lhs.data[i][k] * rhs.data[k][j]);
            }
            result.data[i][j] = ElementValue;
        }
    }
    return result;
}

inline void
operator *= (float4x4 &lhs, float4x4 rhs) {
    lhs = lhs * rhs;
}

inline float4x4
operator * (float4x4 matrix, float scalar) {
    float4x4 result;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            result.data[i][j] = matrix.data[i][j] * scalar;
        }
    }
    return result;
}

inline float4x4
operator * (float scalar, float4x4 matrix) {
    float4x4 result;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            result.data[i][j] = matrix.data[i][j] * scalar;
        }
    }
    return result;
}

inline float3
operator * (float3 vector, float3x3 matrix) {
    float3 result;
    result.x = vector.x * matrix.data[0][0] + vector.y * matrix.data[1][0] + vector.z * matrix.data[2][0];
    result.y = vector.x * matrix.data[0][1] + vector.y * matrix.data[1][1] + vector.z * matrix.data[2][1];
    result.z = vector.x * matrix.data[0][2] + vector.y * matrix.data[1][2] + vector.z * matrix.data[2][2];
    return result;
}

// NOTE(Alex): Assumes the matrix's fourth column is set to (0, 0, 0, 1) for accurate results.
inline float4
operator * (float4 vector, float4x4 matrix) {
    float4 result;
    result.x = vector.x * matrix.data[0][0] + vector.y * matrix.data[1][0] +
        vector.z * matrix.data[2][0] + vector.w * matrix.data[3][0];
    result.y = vector.x * matrix.data[0][1] + vector.y * matrix.data[1][1] +
        vector.z * matrix.data[2][1] + vector.w * matrix.data[3][1];
    result.z = vector.x * matrix.data[0][2] + vector.y * matrix.data[1][2] +
        vector.z * matrix.data[2][2] + vector.w * matrix.data[3][2];
    result.w = vector.x * matrix.data[0][3] + vector.y * matrix.data[1][3] +
        vector.z * matrix.data[2][3] + vector.w * matrix.data[3][3];
    return result;
}

inline float4x4
operator + (float4x4 lhs, float4x4 rhs) {
    float4x4 result;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            result.data[i][j] = lhs.data[i][j] + rhs.data[i][j];
        }
    }
    return result;
}

inline float4x4
operator - (float4x4 lhs, float4x4 rhs) {
    float4x4 result;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            result.data[i][j] = lhs.data[i][j] - rhs.data[i][j];
        }
    }
    return result;
}

inline float
dot(float3 lhs, float3 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

static float3
cross(float3 lhs, float3 rhs) {
    float3 result;
    result.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
    result.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
    result.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
    return result;
}

inline float
magnitude(float3 source) {
    return sqrtf(source.x * source.x + source.y * source.y + source.z * source.z);
}

inline float3
normalize(float3 source) {
    if(source == float3()) {
        return source;
    }
    float3 result;
    float source_length = magnitude(source);
    float multiplier = 1.0f / source_length;
    
    result.x = source.x * multiplier;
    result.y = source.y * multiplier;
    result.z = source.z * multiplier;
    
    return result;
}

inline float
find_scalar_multiple(float3 a, float3 b) {
    float EPSILON = 0.000001f;
    float cross_product_magnitude = magnitude(cross(a, b));
    if((cross_product_magnitude < -EPSILON) || (cross_product_magnitude > EPSILON)) {
        return 0.0f;
    }
    if(b.x != 0.0f) {
        return a.x / b.x;
    }
    if(b.y != 0.0f) {
        return a.y / b.y;
    }
    if(b.z != 0.0f) {
        return a.z / b.z;
    }
    return 0.0f;
}

static void
scale(float3 *vector, float x, float y, float z) {
    vector->x *= x;
    vector->y *= y;
    vector->z *= z;
}

static void
rotate(float3 *vector, float angle, int axis_of_rotation) {
    float3 vec = *vector;
    switch(axis_of_rotation) {
        case X_AXIS: {
            vector->y = vec.y * (float)cos(angle) + vec.z * -1*(float)sin(angle);
            vector->z = vec.y * (float)sin(angle) + vec.z * (float)cos(angle);
        } break;
        
        case Y_AXIS: {
            vector->x = vec.x * (float)cos(angle) + vec.z * (float)sin(angle);
            vector->z = vec.x * -1*(float)sin(angle) + vec.z * (float)cos(angle);
        } break;
        
        case Z_AXIS: {
            vector->x = vec.x * (float)cos(angle) + vec.y * -1*(float)sin(angle);
            vector->y = vec.x * (float)sin(angle) + vec.y * (float)cos(angle);      
        } break;
    }
}

static void 
rotate(float3 *vector, float angle, float3 *point, float3 *line) { //rotates vector about "line" going through "point"
    float3 vec = *vector;
    vector->x = (point->x*(line->y*line->y + line->z*line->z) - line->x*(point->y*line->y + point->z*line->z - line->x*vec.x - line->y*vec.y - line->z*vec.z)) *
        (1.0f - (float)cos(angle)) + vec.x*(float)cos(angle) + ((-point->z)*line->y + point->y*line->z - line->z*vec.y + line->y*vec.z) * (float)sin(angle);
    
    vector->y = (point->y*(line->x*line->x + line->z*line->z) - line->y*(point->x*line->x + point->z*line->z - line->x*vec.x - line->y*vec.y - line->z*vec.z)) *
        (1.0f - (float)cos(angle)) + vec.y*(float)cos(angle) + (point->z*line->x - point->x*line->z + line->z*vec.x - line->x*vec.z) * (float)sin(angle);
    
    vector->z = (point->z*(line->x*line->x + line->y*line->y) - line->z*(point->x*line->x + point->y*line->y - line->x*vec.x - line->y*vec.y - line->z*vec.z)) *
        (1.0f - (float)cos(angle)) + vec.z*(float)cos(angle) + ((-point->y)*line->x + point->x*line->y - line->y*vec.x + line->x*vec.y) * (float)sin(angle);
}

inline float
dot(float4 lhs, float4 rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

static float4
cross(float4 lhs, float4 rhs) {
    float4 result;
    result.x = (lhs.y * rhs.z) - (lhs.z * rhs.y);
    result.y = (lhs.z * rhs.x) - (lhs.x * rhs.z);
    result.z = (lhs.x * rhs.y) - (lhs.y * rhs.x);
    return result;
}

inline float
magnitude(float4 source) {
    return sqrtf(source.x * source.x + source.y * source.y + source.z * source.z + source.w * source.w);
}

inline float4
normalize(float4 source) {
    if(source == float4()) {
        return source;
    }
    float4 result;
    float source_length = magnitude(source);
    float multiplier = 1.0f / source_length;
    
    result.x = source.x * multiplier;
    result.y = source.y * multiplier;
    result.z = source.z * multiplier;
    result.w = source.w * multiplier;
    
    return result;
}

static void
scale(float4 *vector, float x, float y, float z, float w) {
    vector->x *= x;
    vector->y *= y;
    vector->z *= z;
    vector->w *= w;
}

static void
rotate(float4 *vector, float angle, int axis_of_rotation) {
    switch(axis_of_rotation) {
        case X_AXIS: {
            vector->y = vector->y * (float)cos(angle) + vector->z * -1*(float)sin(angle);
            vector->z = vector->y * (float)sin(angle) + vector->z * (float)cos(angle);
        } break;
        
        case Y_AXIS: {
            vector->x = vector->x * (float)cos(angle) + vector->z * (float)sin(angle);
            vector->z = vector->x * -1*(float)sin(angle) + vector->z * (float)cos(angle);
        } break;
        
        case Z_AXIS: {
            vector->x = vector->x * (float)cos(angle) + vector->y * -1*(float)sin(angle);
            vector->y = vector->x * (float)sin(angle) + vector->y * (float)cos(angle);      
        } break;
    }
}

static float4x4
make_scaling_matrix(float x, float y, float z, float w) {
    return float4x4(x, 0.0f, 0.0f, 0.0f,
                 0.0f, y, 0.0f, 0.0f,
                 0.0f, 0.0f, z, 0.0f,
                 0.0f, 0.0f, 0.0f, w);   
}

static float3x3
transpose(float3x3 matrix) {
    float3x3 result;
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            result.data[i][j] = matrix.data[j][i];
        }       
    }
    return result;
}

static float4x4
transpose(float4x4 matrix) {
    float4x4 result;
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            result.data[i][j] = matrix.data[j][i];
        }       
    }
    return result;
}

inline quat
operator * (quat lhs, quat rhs) {
    quat result;
    float3 complex1 = float3(lhs.x, lhs.y, lhs.z);
    float3 complex2 = float3(rhs.x, rhs.y, rhs.z);
    float3 complex = (complex1 * rhs.w) + (complex2 * lhs.w) + cross(complex1, complex2);

    result.x = complex.x;
    result.y = complex.y;
    result.z = complex.z;
    result.w = (lhs.w * rhs.w) - dot(complex1, complex2);
    return result;
}

inline quat
operator * (float scalar, quat rhs) {
    quat result;
    result.x = scalar * rhs.x;
    result.y = scalar * rhs.y;
    result.z = scalar * rhs.z;
    result.w = scalar * rhs.w;
    return result;
}

inline quat
operator * (quat lhs, float scalar) {
    quat result;
    result.x = lhs.x * scalar;
    result.y = lhs.y * scalar;
    result.z = lhs.z * scalar;
    result.w = lhs.w * scalar;
    return result;
}

inline quat
operator / (quat lhs, float scalar) {
    quat result;
    result.x = lhs.x / scalar;
    result.y = lhs.y / scalar;
    result.z = lhs.z / scalar;
    result.w = lhs.w / scalar;
    return result;
}

inline quat
operator + (quat lhs, quat rhs) {
    quat result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    result.w = lhs.w + rhs.w;
    return result;
}

float magnitude(quat q) {
    return (float)sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
}

quat normalize(quat q) {
    if(q == quat()) {
        return q;
    }
    return q / magnitude(q);
}

quat quat_from_euler_angles(float3 euler_angles) {
    quat result;
    
    float cp = (float)cos(euler_angles.x * 0.5);
    float sp = (float)sin(euler_angles.x * 0.5);
    float cy = (float)cos(euler_angles.y * 0.5);
    float sy = (float)sin(euler_angles.y * 0.5);
    float cr = (float)cos(euler_angles.z * 0.5);
    float sr = (float)sin(euler_angles.z * 0.5);

    result.x = cy * sr * cp - sy * cr * sp;
    result.y = cy * cr * sp + sy * sr * cp;
    result.z = sy * cr * cp - cy * sr * sp;
    result.w = cy * cr * cp + sy * sr * sp;

    return result;
}

float dot(quat lhs, quat rhs) {
    return(lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z + lhs.w*rhs.w);
}

static float4x4
rotation_matrix(quat rotation) {
    return float4x4(1.0f - 2.0f * rotation.y*rotation.y - 2.0f * rotation.z*rotation.z,
                 2.0f * rotation.x * rotation.y - 2.0f * rotation.z * rotation.w,
                 2.0f * rotation.x * rotation.z + 2.0f * rotation.y * rotation.w,
                 0.0f,
                 
                 2.0f * rotation.x * rotation.y + 2.0f * rotation.z * rotation.w,
                 1.0f - 2.0f * rotation.x*rotation.x - 2.0f * rotation.z*rotation.z,
                 2.0f * rotation.y * rotation.z - 2.0f * rotation.x * rotation.w,
                 0.0f,
                 
                 2.0f * rotation.x * rotation.z - 2.0f * rotation.y * rotation.w,
                 2.0f * rotation.y * rotation.z - 2.0f * rotation.x * rotation.w,
                 1.0f - 2.0f * rotation.x*rotation.x - 2.0f * rotation.y*rotation.y,
                 0.0f,

                 0.0f, 0.0f, 0.0f, 1.0f);
}

static float4x4
model_transform(float3 world_pos, quat rotation) {
    float4x4 translation = float4x4(1.0f, 0.0f, 0.0f, world_pos.x,
                              0.0f, 1.0f, 0.0f, world_pos.y,
                              0.0f, 0.0f, 1.0f, world_pos.z,
                              0.0f, 0.0f, 0.0f, 1.0f);

    float4x4 rot = float4x4(1.0f - 2.0f * rotation.y*rotation.y - 2.0f * rotation.z*rotation.z,
                      2.0f * rotation.x * rotation.y - 2.0f * rotation.z * rotation.w,
                      2.0f * rotation.x * rotation.z + 2.0f * rotation.y * rotation.w,
                      0.0f,
                 
                      2.0f * rotation.x * rotation.y + 2.0f * rotation.z * rotation.w,
                      1.0f - 2.0f * rotation.x*rotation.x - 2.0f * rotation.z*rotation.z,
                      2.0f * rotation.y * rotation.z - 2.0f * rotation.x * rotation.w,
                      0.0f,
                 
                      2.0f * rotation.x * rotation.z - 2.0f * rotation.y * rotation.w,
                      2.0f * rotation.y * rotation.z - 2.0f * rotation.x * rotation.w,
                      1.0f - 2.0f * rotation.x*rotation.x - 2.0f * rotation.y*rotation.y,
                      0.0f,

                      0.0f, 0.0f, 0.0f, 1.0f);

    return translation * rot;
};

static float4x4
view_transform(float3 position, float3 direction, float3 up) {
    up = normalize(up);
    float3 forward = normalize(direction);
    float3 right = normalize(cross(up, forward)); 

    
    float4x4 translation = float4x4(1.0f, 0.0f, 0.0f, -position.x,
                              0.0f, 1.0f, 0.0f, -position.y,
                              0.0f, 0.0f, 1.0f, -position.z,
                              0.0f, 0.0f, 0.0f,  1.0f);

    float4x4 orientation = float4x4(right.x,   right.y,   right.z,   0.0f,
                              up.x,      up.y,      up.z,      0.0f,
                              forward.x, forward.y, forward.z, 0.0f,
                              0.0f,      0.0f,      0.0f,      1.0f);

    return orientation * translation;
}

static float4x4
perspective(float FOV, float aspect, float z_near, float z_far) {
    float4x4 result;
    float tan_half_FOV = (float)tan(FOV * 0.5 * (float)PI / 180.0f); 
    result.data[0][0] = 1.0f / (tan_half_FOV * aspect); 
    result.data[1][1] = 1.0f / tan_half_FOV; 
    result.data[2][2] = (z_far + z_near) / (z_far - z_near); 
    result.data[3][2] = -(2.0f * z_far * z_near) / (z_far - z_near);
    result.data[2][3] = 1.0f;
    return result;
}

inline float
lerp(float a, float b, float t) {
    return t*b + (1.0f - t)*a;
}

inline float2
lerp(float2 a, float2 b, float t) {
    return t*b + (1.0f - t)*a;
}

inline float3
lerp(float3 a, float3 b, float t) {
    return t*b + (1.0f - t)*a;
}

inline quat
lerp(quat a, quat b, float t) {
    return normalize(t*b + (1.0f - t)*a);
}

inline quat
shortest_lerp(quat a, quat b, float t) {
    return normalize(sign(dot(a, b)) * (1.0f - t)*a + t*b);
}

inline bool
ray_intersects_triangle(float3 ro, float3 rd, float3 v0, float3 v1, float3 v2, float3 &intersection) {
    
    const float EPSILON = 0.0000001f; 
    float3 vertex0 = v0;
    float3 vertex1 = v1;
    float3 vertex2 = v2;
    float3 edge1, edge2, h, s, q;
    float a,f,u,v;
    edge1 = vertex1 - vertex0;
    edge2 = vertex2 - vertex0;
    h = cross(rd, edge2);
    a = dot(edge1, h);
    if (a > -EPSILON && a < EPSILON)
        return false;
    f = 1/a;
    s = ro - vertex0;
    u = f * dot(s, h);
    if (u < 0.0 || u > 1.0)
        return false;
    q = cross(s, edge1);
    v = f * dot(rd, q);
    if (v < 0.0 || u + v > 1.0)
        return false;
    // At this stage we can compute t to find out where the intersection point is on the line.
    float t = f * dot(edge2, q);
    if (t > EPSILON) // ray intersection
    {
        intersection = ro + rd * t; 
        return true;
    }
    else // This means that there is a line intersection but not a ray intersection.
        return false;
}

inline float
abs(float x) {
    return (x < 0)? -x : x;
}

inline int
abs(int x) {
    return (x < 0)? -x : x;
}

inline float
floor(float x) {
    return (x < 0) ? ((float)(int)x - 1.0f) : ((float)(int)x);
}

inline float
ceil(float x) {
    return (x < 0) ? ((float)(int)x) : ((float)(int)x + 1.0f);
}

inline float
dtr(float degrees) {
    return (degrees / 180.0f) * (float)PI;
}
