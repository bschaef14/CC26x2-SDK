/*!
 *  ======== SymbolTable.xdc ========
 *  @_nodoc
 */
metaonly module SymbolTable inherits xdc.rov.ISymbolTable {

instance:

    /*!
     *  ======== create ========
     *  Create takes the handle to an xdc.rov.ISymbolTable Java object.
     */
    create(Any elf);

    /*
     *  ======== whatis ========
     *  Return the largest valued symbol that's also <= addr
     *
     *  @returns - "" in the event that the symbol table does not support
     *             this operation
     *           - "null" in the event that all nm addresses are > addr; i.e.,
     *             addr is less than all addrs in the symbol table
     *           - the name of the symbol whose addr is the largest such that
     *             it's also <= addr
     */
    String whatis(ULong addr);
}
