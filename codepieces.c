

void pinn_sync_info_to_sb(struct pinn_super_block *sb, struct pinn_super_info *si){
    int i;

    sb->s_block_size = si->s_block_size;
    sb->s_inodes_count = si->s_inodes_count;
    sb->s_blocks_count = si->s_blocks_count;
    sb->s_r_blocks_count = si->s_r_blocks_count;
    sb->s_free_blocks_count = si->s_free_blocks_count;
    sb->s_free_inodes_count = si->s_free_inodes_count;
    sb->s_first_data_block = si->s_first_data_block;


    for (i = 0; i < IBITMAPSIZE; ++i)
        sb->s_inode_bitmap[i] = si->s_inode_bitmap[i];

    for (i = 0; i < BBITMAPSIZE; ++i)
        sb->s_block_bitmap[i] = si->s_block_bitmap[i];

    for (i = 0; i < PADSIZE; ++i)
        sb->s_pad[i] = si->s_pad[i];
}

void pinn_sync_sb_to_info(struct pinn_super_info *si, struct pinn_super_block *sb){
    int i;

    si->s_block_size = sb->s_block_size;
    si->s_inodes_count = sb->s_inodes_count;
    si->s_blocks_count = sb->s_blocks_count;
    si->s_r_blocks_count = sb->s_r_blocks_count;
    si->s_free_blocks_count = sb->s_free_blocks_count;
    si->s_free_inodes_count = sb->s_free_inodes_count;
    si->s_first_data_block = sb->s_first_data_block;


    for (i = 0; i < IBITMAPSIZE; ++i)
        si->s_inode_bitmap[i] = sb->s_inode_bitmap[i];

    for (i = 0; i < BBITMAPSIZE; ++i)
        si->s_block_bitmap[i] = sb->s_block_bitmap[i];

    for (i = 0; i < PADSIZE; ++i)
        si->s_pad[i] = sb->s_pad[i];
}
