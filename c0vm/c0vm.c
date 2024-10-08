

/**************************************************************************/
/*              COPYRIGHT Carnegie Mellon University 2023                 */
/* Do not post this file or any derivative on a public site or repository */
/**************************************************************************/
#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "lib/xalloc.h"
#include "lib/stack.h"
#include "lib/contracts.h"
#include "lib/c0v_stack.h"
#include "lib/c0vm.h"
#include "lib/c0vm_c0ffi.h"
#include "lib/c0vm_abort.h"

/* call stack frames */
typedef struct frame_info frame;
struct frame_info {
  c0v_stack_t  S;   /* Operand stack of C0 values */
  ubyte       *P;   /* Function body */
  size_t       pc;  /* Program counter */
  c0_value    *V;   /* The local variables */
};


int execute(struct bc0_file *bc0) {
  REQUIRES(bc0 != NULL);

  /* Variables */
  c0v_stack_t S = c0v_stack_new(); /* Operand stack of C0 values */
  ubyte *P = bc0->function_pool->code;      /* Array of bytes that make up the current function */
  size_t pc = 0;     /* Current location within the current byte array P */
  c0_value *V = xmalloc(sizeof(c0_value) * bc0->function_pool->num_vars);   /* Local variables (you won't need this till Task 2) */
  (void) V;      // silences compilation errors about V being currently unused

  /* The call stack, a generic stack that should contain pointers to frames */
  /* You won't need this until you implement functions. */
 // gstack_t callStack;
//(void) callStack; // silences compilation errors about callStack being currently unused
  gstack_t callStack = stack_new();

  while (true) {

//#ifdef DEBUG
    /* You can add extra debugging information here */
    /*fprintf(stderr, "Opcode %x -- Stack size: %zu -- PC: %zu\n",
            P[pc], c0v_stack_size(S), pc);*/
//#endif


    switch (P[pc]) {

    /* Additional stack operation: */
    //TASK 1

    case POP: {
      pc++;
      c0v_pop(S);
      break;
    }

    case DUP: {
      pc++;
      c0_value v = c0v_pop(S);
      c0v_push(S,v);
      c0v_push(S,v);
      break;
    }

    case SWAP: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      c0v_push(S, V1);
      c0v_push(S, V2);
      break;
    }


    /* Returning from a function.
     * This currently has a memory leak! You will need to make a slight
     * change for the initial tasks to avoid leaking memory.  You will
     * need to revise it further when you write INVOKESTATIC. */

    case RETURN: {
      c0_value retval = c0v_pop(S);
      ASSERT(c0v_stack_empty(S));

      c0v_stack_free(S);
      free(V);
  
      if(stack_empty(callStack)){
        stack_free(callStack, &free);
      }

      else{ 
        frame* FRAME = pop(callStack);
        S = FRAME->S;
        V = FRAME->V;
        pc = FRAME->pc;
        P = FRAME->P;
        c0v_push(S, retval);
        free(FRAME);
        break;
      }
      return val2int(retval);
    }


    /* Arithmetic and Logical operations */

    case IADD: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      int res = (v1 + v2);
      c0v_push(S, int2val(res));
      break;
    }

    case ISUB: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      int res = (v2 - v1);
      c0v_push(S, int2val(res));
      break;
    }

    case IMUL: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);

      int res = (v1 * v2);
      c0v_push(S, int2val(res));
      break;
    }

    case IDIV: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      if (v1 == 0) {
        c0_arith_error("divided by 0");
      } 
      if (v2 == INT_MIN && v1 == -1) {
        c0_arith_error("divide INT_MIN by -1");
      }
      int res = (v2 / v1);
      c0v_push(S, int2val(res));
      break;
    }

    case IREM: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      if (v1 == 0) {
        c0_arith_error("divided by 0");
      } 
      if (v2 == INT_MIN && v1 == -1) {
        c0_arith_error("divide INT_MIN by -1");
      }
      int res = (v2 % v1);
      c0v_push(S, int2val(res));
      break;
    }

    case IAND: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      int res = (v1 & v2);
      c0v_push(S, int2val(res));
      break;
    }

    case IOR: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      int res = (v1 | v2);
      c0v_push(S, int2val(res));
      break;
    }

    case IXOR: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      int res = (v1 ^ v2);
      c0v_push(S, int2val(res));
      break;
    }

    case ISHR: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      if(v1 < 0) {
        c0_arith_error("cannot shift right");
      }
      if(v1 >= 32) {
        c0_arith_error("cannot shift right");
      }
      int res = (v2 >> v1);
      c0v_push(S, int2val(res));
      break;
    }

    case ISHL: {
      pc++;
      c0_value V1 = c0v_pop(S);
      c0_value V2 = c0v_pop(S);
      int v1 = val2int(V1);
      int v2 = val2int(V2);
      if(v1 < 0) {
        c0_arith_error("cannot shift left");
      }
      if(v1 >= 32) {
        c0_arith_error("cannot shift left");
      }
      int res = (v2 << v1);
      c0v_push(S, int2val(res));
      break;
    }


    /* Pushing constants */

    case BIPUSH: {
      pc++;
      int8_t byteOperand = (int8_t)P[pc];
      pc++;
      c0v_push(S, int2val((int32_t)byteOperand));
      break;
    }

    case ILDC: {
      pc++;
      uint8_t highByte = P[pc];
      pc++;
      uint8_t lowByte = P[pc];
      pc++;
      uint16_t poolIndex = (highByte << 8) | lowByte;
      if (poolIndex > bc0->int_count) {
        c0_arith_error("Invalid integer pool index");
      }
      c0v_push(S, int2val(bc0->int_pool[poolIndex]));
      break;
    }

    case ALDC: {
      pc++;
      uint8_t highByte = P[pc];
      pc++;
      uint8_t lowByte = P[pc];
      pc++;
      uint16_t stringIndex = (highByte << 8) | lowByte;
      if (stringIndex > bc0->string_count) {
        c0_arith_error("String pool index exceeded");
      }
      c0v_push(S, ptr2val(&bc0->string_pool[stringIndex]));
      break;
    }

    case ACONST_NULL: {
      pc++;
      c0v_push(S, ptr2val(NULL));
      break;
    }


    /* Operations on local variables */

    case VLOAD: {
      pc++;
      c0v_push(S, V[P[pc]]);
      pc++;
      break;
    }

    case VSTORE: {
      pc++;
      V[P[pc]] = c0v_pop(S);
      pc++;
      break;
    }


    /* Assertions and errors */

    case ATHROW: {
      pc++;
      c0_user_error(val2ptr(c0v_pop(S)));
      break;
    }

    case ASSERT: {
      pc++;
      c0_value V1 = c0v_pop(S);
      char *assertText = val2ptr(V1);
      c0_value V2 = c0v_pop(S);
      int v = val2int(V2);
      if (v == 0) {
        c0_assertion_failure(assertText);
      }
      break;
    }


    /* Control flow operations */

    case NOP: {
      pc++;
      break;
    }

    case IF_CMPEQ: {
      ubyte highByte = (P[pc + 1]);
      ubyte lowByte = (P[pc + 2]);
      int16_t result = (int16_t) (highByte << 8 | lowByte);
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      pc += (val_equal(v1, v2)) ? result : 3;
      break;
    }

    case IF_CMPNE: {
      ubyte highByte = P[pc + 1];
      ubyte lowByte = P[pc + 2];
      int16_t result = (int16_t)((highByte << 8) | lowByte);
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      pc += (!val_equal(v1, v2)) ? result : 3;
      break;
    }

    case IF_ICMPLT: {
      ubyte highByte = P[pc + 1];
      ubyte lowByte = P[pc + 2];
      int16_t result = (int16_t)((highByte << 8) | lowByte);
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      int32_t V1 = val2int(v1);
      int32_t V2 = val2int(v2);
      pc += (V2 < V1) ? result : 3;
      break;
    }

    case IF_ICMPGE: {
      ubyte highByte = P[pc + 1];
      ubyte lowByte = P[pc + 2];
      int16_t result = (int16_t)((highByte << 8) | lowByte);
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      int32_t V1 = val2int(v1);
      int32_t V2 = val2int(v2);
      pc += (!(V2 < V1)) ? result : 3;
      break;
    }

    case IF_ICMPGT: {
      ubyte highByte = P[pc + 1];
      ubyte lowByte = P[pc + 2];
      int16_t result = (int16_t)((highByte << 8) | lowByte);
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      int32_t V1 = val2int(v1);
      int32_t V2 = val2int(v2);
      pc += (V2 > V1) ? result : 3;
      break;
    }

    case IF_ICMPLE: {
      ubyte highByte = P[pc + 1];
      ubyte lowByte = P[pc + 2];
      int16_t result = (int16_t)((highByte << 8) | lowByte);
      c0_value v1 = c0v_pop(S);
      c0_value v2 = c0v_pop(S);
      int32_t V1 = val2int(v1);
      int32_t V2 = val2int(v2);
      pc += (!(V2 > V1)) ? result : 3;
      break;
    }

    case GOTO: {
      ubyte highByte = P[pc + 1];
      ubyte lowByte = P[pc + 2];
      int16_t result = (int16_t)((highByte << 8) | lowByte);
      pc += result;
      break;
    }

///////////////////////////////////
    /* Function call operations: */
    case INVOKESTATIC: {
      ubyte highByte = P[pc + 1];
      ubyte lowByte = P[pc + 2];
      uint16_t function_index = (uint16_t)((highByte << 8) | lowByte);

      struct function_info *function = bc0->function_pool+function_index;

      frame *current_frame = xmalloc(sizeof(frame));
      current_frame->S = S;
      current_frame->P = P;
      current_frame->pc = pc + 3;  // Set to pc after the INVOKESTATIC instruction
      current_frame->V = V;
      push(callStack, current_frame);
    
      V = xmalloc(sizeof(c0_value) * function->num_vars); 
      for (int i = function->num_args - 1; i >= 0; i--) {
          V[i] = c0v_pop(S);
      }

      S = c0v_stack_new();  
      P = function->code;  
      pc = 0;             

      break;
    }

    case INVOKENATIVE: {
      ubyte highByte = P[pc + 1];
      ubyte lowByte = P[pc + 2];
      uint16_t index = (uint16_t)((highByte << 8) | lowByte);

      struct native_info *nativeFuncInfo = bc0->native_pool+index;

      c0_value *A = xcalloc(nativeFuncInfo->num_args, sizeof(c0_value));
      for (int i = nativeFuncInfo->num_args - 1; i >= 0; i--) {
          A[i] = c0v_pop(S);
      }

      c0_value result = (*native_function_table[nativeFuncInfo->function_table_index])(A);
      c0v_push(S, result);
      free(A);

      pc += 3;  // Move pc past the INVOKENATIVE instruction and its operands

      break;
    }


    /* Memory allocation and access operations: */

    case NEW: {
      ubyte n = P[pc + 1];
      c0v_push(S, ptr2val(xcalloc(1, n)));
      pc += 2;
      break;
    }

    case IMLOAD: {
      int *Array = (int*)val2ptr(c0v_pop(S));

      if (Array == NULL) {
        c0_memory_error("point to null");
      }

      c0v_push(S, int2val(*Array));
      pc++;
      break;
    }

    case IMSTORE: {
      int value = val2int(c0v_pop(S));
      int *index = (int*)val2ptr(c0v_pop(S));

      if (index == NULL) {
        c0_memory_error("point to null");
      }

      *index = value;
      pc++;
      break;
    }

    case AMLOAD: {
      void **index = val2ptr(c0v_pop(S));
      if (index == NULL) {
        c0_memory_error("point to null");
      }
      c0v_push(S, ptr2val(*index));
      pc++;
      break;
    }

    case AMSTORE: {
      void *value = val2ptr(c0v_pop(S));
      void *index = val2ptr(c0v_pop(S));
      if (index == NULL) {
        c0_memory_error("point to null");
      }

      *(void **)index = value;
      pc++;
      break;
    }

    case CMLOAD: {
      char *index = (char*)val2ptr(c0v_pop(S));
      if (index == NULL) {
        c0_memory_error("point to null");
      }
      int x = *index;
      c0v_push(S, int2val(x));
      pc++;
      break;
    }

    case CMSTORE: {
      int value = val2int(c0v_pop(S)) & 0x7f;
      char *index = val2ptr(c0v_pop(S));
      if (index == NULL) {
        c0_memory_error("point to null");
      }
      //*index = value & 0x7f;
      *index = value;
      pc++;
      break;
    }

    case AADDF: {
      pc++;
      ubyte offset = P[pc];
      pc++;
      char *base = (char*)val2ptr(c0v_pop(S));

      if (base == NULL) {
        c0_memory_error("Base pointer is null");
      }

      char *field_addr = base + offset;

      c0v_push(S, ptr2val(field_addr));
      
      break;
    }


    /* Array operations: */

    case NEWARRAY: {
      pc++;
      ubyte type = P[pc];
      pc++;

      int Size = val2int(c0v_pop(S));
      if (Size < 0) {
        c0_memory_error("Negative array size");
      }

      c0_array *Array = xmalloc(sizeof(c0_array));
      Array->count = Size;
      Array->elt_size = type;
      Array->elems = xcalloc(Size, type);

      c0v_push(S, ptr2val((void*)Array)); 
      break;
    }

    case ARRAYLENGTH: {
      pc++;
      c0_array *Array = val2ptr(c0v_pop(S));

      if (Array == NULL) {
        c0_memory_error("Null array pointer");
      }
      c0v_push(S, int2val(Array->count));
      break;

    }

    case AADDS: {
      pc++;
      int index = val2int(c0v_pop(S));
      c0_array *Array = val2ptr(c0v_pop(S));

      if (Array == NULL) {
        c0_memory_error("point to null");
      }

      if (index < 0) {
        c0_memory_error("invalid");
      }
      //int
      int C = Array->count;
      if (C <= index) {
        c0_memory_error("invalid");
      }
      
      char *base = (char*)Array->elems;
  
      c0v_push(S, ptr2val(&base[index*Array->elt_size]));

      break;
    }


    /* BONUS -- C1 operations */

    case CHECKTAG:

    case HASTAG:

    case ADDTAG:

    case ADDROF_STATIC:

    case ADDROF_NATIVE:

    case INVOKEDYNAMIC:

    default:
      fprintf(stderr, "invalid opcode: 0x%02x\n", P[pc]);
      abort();
    }
  }

  /* cannot get here from infinite loop */
  assert(false);
}





