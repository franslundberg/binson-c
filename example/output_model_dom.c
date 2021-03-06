/**
 *  This example demonstrates basic hi-level binson library API
 *  to build DOM model and generate binary output via 'binson_serialize()'
 */

#include <stdio.h>
#include "binson/binson.h"
#include "common.h"

int main()
{
    binson          *context;
    binson_writer   *writer;
    binson_parser   *parser;

    binson_io       *err_io, *in, *out, *fio;
    binson_res       res;

    /* DEBUG: disable stdout buffering for debugging purposes */
    setvbuf(stdout, NULL, _IONBF, 0);

    res = binson_io_new( &err_io );
    res = binson_io_new( &in );
    res = binson_io_new( &out );
    res = binson_io_new( &fio );

    res = binson_io_attach_stream( err_io, stdout );
    res = binson_io_attach_stream( in, stdin );
    res = binson_io_attach_stream( out, stdout );
    res = binson_io_open_file( fio, "./output_model_dom.bson", BINSON_IO_MODE_WRITE | BINSON_IO_MODE_CREATE );

    res = binson_writer_new( &writer );
    res = binson_writer_init( writer, out, BINSON_WRITER_FORMAT_JSON_NICE );

    res = binson_parser_new( &parser );
    res = binson_parser_init( parser, in, BINSON_PARSER_MODE_DOM );

    res = binson_new( &context );
    res = binson_init( context, err_io );

    /* ready to build DOM */
    gen_sample_data_binson( context, binson_get_root( context ) );
    
#ifdef DEBUG        
     printf("\n\n------ postorder tree traversal (e.g. to debug tree deletion)  ---------\n");
     binson_traverse( context, binson_get_root(context), BINSON_TRAVERSE_PREORDER, BINSON_DEPTH_LIMIT, binson_cb_dump_debug, NULL );
     printf("---------------\n");
#endif
     
    /* serialize via attached 'binson_writer' */
    res = binson_serialize( context, writer, NULL );
    res = binson_io_printf( out, "\n---------------\n" );

    res = binson_writer_set_format( writer, BINSON_WRITER_FORMAT_HEX );
    res = binson_serialize( context, writer, NULL );

    /* save to file as raw binson format */
    res = binson_writer_set_format( writer, BINSON_WRITER_FORMAT_RAW );
    res = binson_writer_set_io( writer, fio );
    res = binson_serialize( context, writer, NULL );

    /* we are done. freeing resources */
    res = binson_free( context );
    res = binson_parser_free( parser );
    res = binson_writer_free( writer );

    res = binson_io_free( err_io );
    res = binson_io_free( in );
    res = binson_io_free( out );
    res = binson_io_free( fio );

   return res;
}
