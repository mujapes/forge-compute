#include <acl/core/ansi_allocator.h>
#include <acl/compression/compress.h>
#include <acl/compression/track.h>
#include <acl/compression/track_array.h>
#include <acl/compression/transform_error_metrics.h>
#include <acl/compression/output_stats.h>
#include <cstdio>
#include <vector>
#include <cmath>
#include <cstring>
extern "C" int acl_sample(const void*,uint32_t,float,uint32_t,const float*,uint32_t,float*,uint32_t);
int main(){
 acl::ansi_allocator allocator;acl::track_array_qvvf tracks(allocator,3);
 for(uint32_t i=0;i<3;i++){
  acl::track_desc_transformf desc;desc.output_index=i;desc.parent_index=i?0:acl::k_invalid_track_index;
  tracks[i]=acl::track_qvvf::make_reserve(desc,allocator,31,30);
  for(uint32_t s=0;s<31;s++){
   const float t=float(s)/30,angle=i==0?t*.4f:0;
   rtm::qvvf q;q.rotation=rtm::quat_set(0,0,std::sin(angle),std::cos(angle));
   q.translation=rtm::vector_set(i==0?10*t:float(i),i==1?t*t:0,i==2?-t:0);
   q.scale=rtm::vector_set(i==2?1+t*.1f:1,1,1);tracks[i][s]=q;
  }
 }
 acl::qvvf_transform_error_metric metric;auto settings=acl::get_default_compression_settings();settings.error_metric=&metric;
 acl::compressed_tracks* compressed=nullptr;acl::output_stats stats;
 auto err=acl::compress_track_list(allocator,tracks,settings,compressed,stats);
 if(err.any()){std::fprintf(stderr,"%s\n",err.c_str());return 1;}
 FILE* bin=std::fopen("synthetic.acl","wb");std::fwrite(compressed,1,compressed->get_size(),bin);std::fclose(bin);
 float defaults[30]={};for(int i=0;i<3;i++){defaults[i*10+3]=1;defaults[i*10+7]=defaults[i*10+8]=defaults[i*10+9]=1;}
 FILE* expected=std::fopen("expected.f32","wb");
 for(uint32_t rounding=0;rounding<4;rounding++)for(uint32_t i=0;i<257;i++){
  float output[30];if(acl_sample(compressed,compressed->get_size(),float(i)/256,rounding,defaults,30,output,30)!=3)return 2;
  for(float v:output)if(!std::isfinite(v))return 3;
  std::fwrite(output,sizeof(float),30,expected);
 }
 std::fclose(expected);allocator.deallocate(compressed,compressed->get_size());return 0;
}
