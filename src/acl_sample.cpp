// Generic ACL qvvf sampling boundary. No engine/game types or data.
#include <acl/core/compressed_tracks.h>
#include <acl/decompression/decompress.h>
#include <cmath>
#include <cstdint>
#include <cstring>

struct Output final : acl::track_writer {
 const float* defaults;
 float* values;
 Output(const float* d,float* v):defaults(d),values(v){}
 static constexpr acl::default_sub_track_mode get_default_rotation_mode(){return acl::default_sub_track_mode::variable;}
 static constexpr acl::default_sub_track_mode get_default_translation_mode(){return acl::default_sub_track_mode::variable;}
 static constexpr acl::default_sub_track_mode get_default_scale_mode(){return acl::default_sub_track_mode::variable;}
 rtm::quatf RTM_SIMD_CALL get_variable_default_rotation(uint32_t i)const {const float* p=defaults+10*i;return rtm::quat_set(p[0],p[1],p[2],p[3]);}
 rtm::vector4f RTM_SIMD_CALL get_variable_default_translation(uint32_t i)const {const float* p=defaults+10*i+4;return rtm::vector_set(p[0],p[1],p[2]);}
 rtm::vector4f RTM_SIMD_CALL get_variable_default_scale(uint32_t i)const {const float* p=defaults+10*i+7;return rtm::vector_set(p[0],p[1],p[2]);}
 void RTM_SIMD_CALL write_rotation(uint32_t i,rtm::quatf_arg0 q){float* p=values+10*i;p[0]=rtm::quat_get_x(q);p[1]=rtm::quat_get_y(q);p[2]=rtm::quat_get_z(q);p[3]=rtm::quat_get_w(q);}
 void RTM_SIMD_CALL write_translation(uint32_t i,rtm::vector4f_arg0 v){float* p=values+10*i+4;p[0]=rtm::vector_get_x(v);p[1]=rtm::vector_get_y(v);p[2]=rtm::vector_get_z(v);}
 void RTM_SIMD_CALL write_scale(uint32_t i,rtm::vector4f_arg0 v){float* p=values+10*i+7;p[0]=rtm::vector_get_x(v);p[1]=rtm::vector_get_y(v);p[2]=rtm::vector_get_z(v);}
};
extern "C" int acl_validate(const void* bytes,uint32_t length){
 if(!bytes||length<sizeof(acl::compressed_tracks)||length>16000000||(reinterpret_cast<uintptr_t>(bytes)&15))return -1;
 uint32_t size;std::memcpy(&size,bytes,4);if(size!=length)return -2;
 auto& tracks=*static_cast<const acl::compressed_tracks*>(bytes);
 if(tracks.is_valid(true).any())return -3;
 if(tracks.get_track_type()!=acl::track_type8::qvvf||tracks.get_num_tracks()>10000||tracks.get_num_samples_per_track()>100000)return -4;
 return static_cast<int>(tracks.get_num_tracks());
}
// All arrays are contiguous track order: xyzw, xyz translation, xyz scale.
// Defaults are supplied explicitly. Rounding: 0=interpolate,1=nearest,2=floor,3=ceil.
extern "C" int acl_sample(const void* bytes,uint32_t length,float seconds,uint32_t rounding,const float* defaults,uint32_t default_count,float* output,uint32_t output_count){
 const int count=acl_validate(bytes,length);if(count<0)return count;
 if(!std::isfinite(seconds)||rounding>3||!defaults||!output||default_count<uint32_t(count)*10||output_count<uint32_t(count)*10)return -5;
 for(uint32_t i=0;i<uint32_t(count)*10;i++)if(!std::isfinite(defaults[i]))return -6;
 acl::decompression_context<acl::decompression_settings> context;
 if(!context.initialize(*static_cast<const acl::compressed_tracks*>(bytes)))return -7;
 const acl::sample_rounding_policy policies[]={acl::sample_rounding_policy::none,acl::sample_rounding_policy::nearest,acl::sample_rounding_policy::floor,acl::sample_rounding_policy::ceil};
 context.seek(seconds,policies[rounding]);Output writer(defaults,output);context.decompress_tracks(writer);return count;
}
