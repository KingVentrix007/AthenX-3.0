/********************************************************************
** Copyright (c) 2018-2020 Guan Wenliang, Stephan Hadinger
** This file is part of the Berry default interpreter.
** skiars@qq.com, https://github.com/Skiars/berry
** See Copyright Notice in the LICENSE file or at
** https://github.com/Skiars/berry/blob/master/LICENSE
********************************************************************/
#include "berry/be_object.h"
#include "berry/be_module.h"
#include "berry/be_string.h"
#include "berry/be_vector.h"
#include "berry/be_class.h"
#include "berry/be_debug.h"
#include "berry/be_map.h"
#include "berry/be_vm.h"
#include "berry/be_exec.h"
#include "berry/be_gc.h"
#include <string.h>


#if !BE_USE_PRECOMPILED_OBJECT
be_native_module_attr_table(undefined) {
    be_native_module_nil(".p"),         /* not needed but can't be empty */
};

be_define_native_module(undefined, NULL);
#else
/* @const_object_info_begin
module undefined (scope: global) {
    .p, nil()
}
@const_object_info_end */
#include "../generate/be_fixed_undefined.h"
#endif
