import fs from 'node:fs';
import assert from 'node:assert/strict';
import create from '../dist/acl-sample.mjs';
const m=await create({wasmBinary:fs.readFileSync(new URL('../dist/acl-sample.wasm',import.meta.url))}),bytes=fs.readFileSync('synthetic.acl'),expected=fs.readFileSync('expected.f32');
const allocation=m._malloc(bytes.length+16),pointer=(allocation+15)&~15;
m.HEAPU8.set(bytes,pointer);
assert.equal(m._acl_validate(pointer,bytes.length),3);
assert.equal(m._acl_validate(pointer,bytes.length-1),-2);
assert.equal(m._acl_validate(pointer+1,bytes.length),-1);
const defaults=m._malloc(120),output=m._malloc(120);
const ref=new Float32Array(30);for(let i=0;i<3;i++){ref[i*10+3]=1;ref[i*10+7]=ref[i*10+8]=ref[i*10+9]=1;}
m.HEAPF32.set(ref,defaults/4);
let index=0,maxError=0;
for(let rounding=0;rounding<4;rounding++)for(let sample=0;sample<257;sample++){
 assert.equal(m._acl_sample(pointer,bytes.length,Math.fround(sample/256),rounding,defaults,30,output,30),3);
 for(let i=0;i<30;i++){
  const a=m.HEAPF32[output/4+i],b=expected.readFloatLE(index++*4),error=Math.abs(a-b);
  assert.ok(Number.isFinite(a)&&error<=2e-5*Math.max(1,Math.abs(b)),`case ${rounding}/${sample}/${i}: ${a} != ${b}`);maxError=Math.max(maxError,error);
 }
}
assert.equal(m._acl_sample(pointer,bytes.length,NaN,0,defaults,30,output,30),-5);
assert.equal(m._acl_sample(pointer,bytes.length,0,4,defaults,30,output,30),-5);
assert.equal(m._acl_sample(pointer,bytes.length,0,0,defaults,29,output,30),-5);
m._free(allocation);m._free(defaults);m._free(output);
console.log(JSON.stringify({samples:1028,components:index,maxError,node:process.version,architecture:process.arch}));
