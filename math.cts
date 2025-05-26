#module: math
#version: 0.1
#desc: simple math ops

define <math::double:function> (x;std::integer) {
  call <std::print:function> with parms (<get;x> + <get;x>)
}