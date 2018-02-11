#include "kheap.h"
#include "paging.h"

u32int placement_address;
heap_t *kheap = NULL;

u32int kmalloc_int(u32int sz, u8int align, u32int *phys) {
  if (kheap != 0) {
    void *addr = alloc(sz, (u8int) align, kheap);
    if (phys != 0) {
      page_t *page = get_page((u32int) addr, 0, kernel_directory);
      *phys = page->frame * 0x1000 + ((u32int) addr & 0xFFF);
    }
    return (u32int) addr;
  }
  //if(align == 1 && (placement_address & 0xfffff000)){//低于4K的不管?
  if (align == 1) {
    placement_address += 0xfff;
    placement_address &= 0xfffff000;
  }
  if (phys) {
    *phys = placement_address;
  }
  u32int ret = placement_address;
  placement_address += sz;

  return ret;
}

u32int kmalloc_a(u32int sz) {
  return kmalloc_int(sz, 1, 0);
}

u32int kmalloc_p(u32int sz, u32int *phys) {
  return kmalloc_int(sz, 0, phys);
}

u32int kmalloc_ap(u32int sz, u32int *phys) {
  return kmalloc_int(sz, 1, phys);
}

u32int kmalloc(u32int sz) {
  return kmalloc_int(sz, 0, 0);
}

void kfree(void *p) {
  free(p, kheap);
}

static s32int find_smallest_hole(u32int size, u8int page_align, heap_t *heap) {
  // Find the smallest hole that will fit.
  u32int iterator = 0;
  while (iterator < heap->index.size) {
    header_t *header = (header_t *) lookup_ordered_array(iterator, &heap->index);
    // If the user has requested the memory be page-aligned
    if (page_align > 0) {
      // Page-align the starting point of this header.
//      u32int location = (u32int) header;
//      s32int offset = 0;
//      if (((location + sizeof(header_t)) & 0xFFFFF000) != 0)
//        offset = 0x1000 /* page size */  - (location + sizeof(header_t)) % 0x1000;
      while (align((u32int) header) - sizeof(header_t) < (u32int) header) {
        header += sizeof(header_t);
      }
      s32int offset = align((u32int) header) + size + sizeof(footer_t);
      s32int hole_size = (s32int) header->size - offset;
      // Can we fit now?
      if (hole_size >= (s32int) size)
        break;
    } else if (header->size >= size)
      break;
    iterator++;
  }
  // Why did the loop exit?
  if (iterator == heap->index.size)
    return -1; // We got to the end and didn't find anything.
  else
    return iterator;
}

static s8int header_t_less_than(void *a, void *b) {
  return (((header_t *) a)->size < ((header_t *) b)->size) ? 1 : 0;
}

/**
 * need :
 *  a heap_t to maintain meta data
 *  a ordered list to maintain free slots
 * @param start -- start address of free memory(AREA)
 * @param end_addr --  end address of free memory(AREA)
 * @param max -- equals to end
 * @param supervisor -- meta data
 * @param readonly -- meta data
 * @return pointer to created heap_t
 *
 *
 * smaller address
 * -------AREA-------
 * index
 * =======
 * heap space
 * start after index on a page aligned address
 * -------AREA-------
 * heap_t
 * points to index in former memory area
 * -------
 * larger address
 */
heap_t *create_heap(u32int start, u32int end_addr, u32int max, u8int supervisor, u8int readonly) {
  heap_t *heap = (heap_t *) kmalloc(sizeof(heap_t));

  // All our assumptions are made on startAddress and endAddress being page-aligned.
  ASSERT(start % 0x1000 == 0);
  ASSERT(end_addr % 0x1000 == 0);

  // Initialise the index.
  heap->index = place_ordered_array((void *) start, HEAP_INDEX_SIZE, &header_t_less_than);

  // Shift the start address forward to resemble where we can start putting data.
  start += sizeof(type_t) * HEAP_INDEX_SIZE;

  // Make sure the start address is page-aligned.
  start = align(start);
  // Write the start, end and max addresses into the heap structure.
  heap->start_address = start;
  heap->end_address = end_addr;
  heap->max_address = max;
  heap->supervisor = supervisor;
  heap->readonly = readonly;

  // We start off with one large hole in the index.
  header_t *hole = (header_t *) start;
  hole->size = end_addr - start;
  hole->magic = HEAP_MAGIC;
  hole->is_hole = 1;

  //create foot
  footer_t *footer = (footer_t *) (end_addr - sizeof(footer_t));
  footer->header = hole;
  footer->magic = HEAP_MAGIC;

  insert_ordered_array((void *) hole, &heap->index);

  return heap;
}

static void expand(u32int new_size, heap_t *heap) {
  PANIC("not using expand");
  // Sanity check.
  ASSERT(new_size > heap->end_address - heap->start_address);
  // Get the nearest following page boundary.
  new_size = align(new_size);
  // Make sure we are not overreaching ourselves.
  ASSERT(heap->start_address + new_size <= heap->max_address);

  // This should always be on a page boundary.
  u32int old_size = heap->end_address - heap->start_address;
  u32int i = old_size;
  while (i < new_size) {
    alloc_frame(get_page(heap->start_address + i, 1, kernel_directory),
                (heap->supervisor) ? 1 : 0, (heap->readonly) ? 0 : 1);
    i += 0x1000 /* page size */;
  }
  heap->end_address = heap->start_address + new_size;
}

static u32int contract(u32int new_size, heap_t *heap) {
  PANIC("not using contract");
  // Sanity check.
  ASSERT(new_size < heap->end_address - heap->start_address);
  // Get the nearest following page boundary.
  new_size = align(new_size);
  // Don't contract too far!
  if (new_size < HEAP_MIN_SIZE)
    new_size = HEAP_MIN_SIZE;
  u32int old_size = heap->end_address - heap->start_address;
  u32int i = old_size - 0x1000;
  while (new_size < i) {
    free_frame(get_page(heap->start_address + i, 0, kernel_directory));
    i -= 0x1000;
  }
  heap->end_address = heap->start_address + new_size;
  return new_size;
}

/**
 *
 * Unhappy fixing bugs :(
 * Unhappy fixing bugs :(
 * Unhappy fixing bugs :(
 * Unhappy fixing bugs :(
 * Unhappy fixing bugs :(
 * Unhappy fixing bugs :(
 * Unhappy fixing bugs :(
 * Unhappy fixing bugs :(
 * Unhappy fixing bugs :(
 *
 *
 *
 *
 * 1.Search the index table to find the smallest hole that will fit the requested size.
 *   As the table is ordered, this just entails iterating through until we find a hole which will fit.
 *     If we didn't find a hole large enough, then:
 *          Expand the heap.
 *          If the index table is empty (no holes have been recorded) then :
 *              add a new entry to it.
 *          Else:
 *              adjust the last header's size member and rewrite the footer.
 *          To ease the number of control-flow statements, we can just recurse and call the allocation function again,
 *          trusting that this time there will be a hole large enough.
 *     Decide if the hole should be split into two parts.
 *     This will normally be the case - we usually will want much less space than is available in the hole.
 *     The only time this will not happen is if there is less free space after allocating the block than the header/footer takes up.
 *     In this case we can just increase the block size and reclaim it all afterwards.
 *
 *     If the block should be page-aligned,
 *     we must alter the block starting address so that it is and create a new hole in the new unused area.
 *     If it is not, we can just delete the hole from the index.
 *     Write the new block's header and footer.
 *     If the hole was to be split into two parts, do it now and write a new hole into the index.
 *     Return the address of the block + sizeof(header_t) to the user.

 * @param size
 * @param page_align
 * @param heap
 * @return
 */
void *alloc(u32int size, u8int page_align, heap_t *heap) {

  // Make sure we take the size of header/footer into account.
  u32int new_size = size + sizeof(header_t) + sizeof(footer_t);
  // Find the smallest hole that will fit.
  s32int iterator = find_smallest_hole(new_size, page_align, heap);

  if (iterator == -1) // If we didn't find a suitable hole
  {
    PANIC("should not reach here");
//    // Save some previous data.
//    u32int old_length = heap->end_address - heap->start_address;
//    u32int old_end_address = heap->end_address;
//
//    // We need to allocate some more space.
//    expand(old_length + new_size, heap);
//    u32int new_length = heap->end_address - heap->start_address;
//
//    // Find the endmost header. (Not endmost in size, but in location).
//    iterator = 0;
//    // Vars to hold the index of, and value of, the endmost header found so far.
//    u32int idx = -1;
//    u32int value = 0x0;
//    while (iterator < heap->index.size) {
//      u32int tmp = (u32int) lookup_ordered_array(iterator, &heap->index);
//      if (tmp > value) {
//        value = tmp;
//        idx = iterator;
//      }
//      iterator++;
//    }
//
//    // If we didn't find ANY headers, we need to add one.
//    if (idx == -1) {
//      header_t *header = (header_t *) old_end_address;
//      header->magic = HEAP_MAGIC;
//      header->size = new_length - old_length;
//      header->is_hole = 1;
//      footer_t *footer = (footer_t *) (old_end_address + header->size - sizeof(footer_t));
//      footer->magic = HEAP_MAGIC;
//      footer->header = header;
//      insert_ordered_array((void *) header, &heap->index);
//    } else {
//      // The last header needs adjusting.
//      header_t *header = lookup_ordered_array(idx, &heap->index);
//      header->size += new_length - old_length;
//      // Rewrite the footer.
//      footer_t *footer = (footer_t *) ((u32int) header + header->size - sizeof(footer_t));
//      footer->header = header;
//      footer->magic = HEAP_MAGIC;
//    }
//    // We now have enough space. Recurse, and call the function again.
//    return alloc(size, page_align, heap);
  }

  header_t *orig_hole_header = (header_t *) lookup_ordered_array(iterator, &heap->index);

  u32int orig_hole_size = orig_hole_header->size;
  /**
   * not enough if we take the mid part and leave two blocks of small memory
   * I won't fix this for now.
   */
  // Here we work out if we should split the hole we found into two parts.
  // Is the original hole size - requested hole size less than the overhead for adding a new hole?
  if (orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)) {
    PANIC("not implemented ");
    // Then just increase the requested size to the size of the hole we found.
//    size += orig_hole_size - new_size;
//    new_size = orig_hole_size;
  }

  // If we need to page-align the data, do it now and make a new hole in front of our block.
//  u32int orig_hole_pos = (u32int) orig_hole_header + sizeof(header_t);
//  if (page_align && (orig_hole_pos & 0xFFF)) {
//    u32int new_location = orig_hole_pos + 0x1000 /* page size */ - (orig_hole_pos & 0xFFF) - sizeof(header_t);
//    header_t *hole_header = (header_t *) orig_hole_pos;
//    hole_header->size =
//        new_location - orig_hole_pos;//0x1000 /* page size */ - (orig_hole_pos & 0xFFF) - sizeof(header_t);
//    hole_header->magic = HEAP_MAGIC;
//    hole_header->is_hole = 1;
//    footer_t *hole_footer = (footer_t *) ((u32int) new_location - sizeof(footer_t));
//    hole_footer->magic = HEAP_MAGIC;
//    hole_footer->header = hole_header;
//    orig_hole_pos = new_location;
//    orig_hole_size = orig_hole_size - hole_header->size;
//  } else {
  // Else we don't need this hole any more, delete it from the index.
  remove_ordered_array(iterator, &heap->index);
//  }

  // Overwrite the original header...
  header_t *block_header = (header_t *) orig_hole_header;
  block_header->magic = HEAP_MAGIC;
  block_header->is_hole = 0;
  block_header->size = new_size;
  // ...And the footer
  footer_t *block_footer = (footer_t *) ((char *) block_header + sizeof(header_t) + size);
  block_footer->magic = HEAP_MAGIC;
  block_footer->header = block_header;

  // We may need to write a new hole after the allocated block.
  // We do this only if the new hole would have positive size...
  if (orig_hole_size - new_size > 0) {
    header_t *hole_header = (header_t *) ((char *) orig_hole_header + sizeof(header_t) + size + sizeof(footer_t));
    hole_header->magic = HEAP_MAGIC;
    hole_header->is_hole = 1;
    hole_header->size = orig_hole_size - new_size;
    footer_t *hole_footer = (footer_t *) ((u32int) hole_header + orig_hole_size - new_size - sizeof(footer_t));
    /**
     * need more assertion
     * should gurantee |addr |hole start ... hole end|
     *                 |addr start ..........addr end|
     *                 matches and not overflow
     *
     */
    if ((u32int) hole_footer < heap->end_address) {
      hole_footer->magic = HEAP_MAGIC;
      hole_footer->header = hole_header;
    } else {
      PANIC("footer address out of limit");
    }
    // Put the new hole in the index;
    insert_ordered_array((void *) hole_header, &heap->index);
  }

  // ...And we're done!
  return (void *) ((u32int) block_header + sizeof(header_t));
}
/**
 * Find the header by taking the given pointer and subtracting the sizeof(header_t).
Sanity checks. Assert that the header and footer's magic numbers remain in tact.
Set the is_hole flag in our header to 1.
If the thing immediately to our left is a footer:
Unify left. In this case, at the end of the algorithm we shouldn't add our header to the hole index (the header we are unifying with is already there!) so set a flag which the algorithm checks later.
If the thing immediately to our right is a header:
Unify right.
If the footer is the last in the heap ( footer_location+sizeof(footer_t) == end_address ):
Contract.
Insert the header into the hole array unless the flag described in Unify left is set.
 * @param p
 * @param heap
 */
void free(void *p, heap_t *heap) {
  // Exit gracefully for null pointers.
  if (p == 0)
    return;

  // Get the header and footer associated with this pointer.
  header_t *header = (header_t *) ((u32int) p - sizeof(header_t));
  footer_t *footer = (footer_t *) ((u32int) header + header->size - sizeof(footer_t));

  // Sanity checks.
  ASSERT(header->magic == HEAP_MAGIC);
  ASSERT(footer->magic == HEAP_MAGIC);

  // Make us a hole.
  header->is_hole = 1;

  // Do we want to add this header into the 'free holes' index?
  char do_add = 1;

  // Unify left
  // If the thing immediately to the left of us is a footer...
  footer_t *test_footer = (footer_t *) ((u32int) header - sizeof(footer_t));
  if (test_footer->magic == HEAP_MAGIC &&
      test_footer->header->is_hole == 1) {
    u32int cache_size = header->size; // Cache our current size.
    header = test_footer->header;     // Rewrite our header with the new one.
    footer->header = header;          // Rewrite our footer to point to the new header.
    header->size += cache_size;       // Change the size.
    do_add = 0;                       // Since this header is already in the index, we don't want to add it again.
  }

  // Unify right
  // If the thing immediately to the right of us is a header...
  header_t *test_header = (header_t *) ((u32int) footer + sizeof(footer_t));
  if (test_header->magic == HEAP_MAGIC &&
      test_header->is_hole) {
    header->size += test_header->size; // Increase our size.
    test_footer = (footer_t *) ((u32int) test_header + // Rewrite it's footer to point to our header.
        test_header->size - sizeof(footer_t));
    footer = test_footer;
    footer->header = header;
    // Find and remove this header from the index.
    u32int iterator = 0;
    while ((iterator < heap->index.size) &&
        (lookup_ordered_array(iterator, &heap->index) != (void *) test_header))
      iterator++;

    // Make sure we actually found the item.
    ASSERT(iterator < heap->index.size);
    // Remove it.
    remove_ordered_array(iterator, &heap->index);
  }

  // If the footer location is the end address, we can contract.
//  if ((u32int) footer + sizeof(footer_t) == heap->end_address) {
//    u32int old_length = heap->end_address - heap->start_address;
//    u32int new_length = contract((u32int) header - heap->start_address, heap);
//    // Check how big we will be after resizing.
//    if (header->size - (old_length - new_length) > 0) {
//      // We will still exist, so resize us.
//      header->size -= old_length - new_length;
//      footer = (footer_t *) ((u32int) header + header->size - sizeof(footer_t));
//      footer->magic = HEAP_MAGIC;
//      footer->header = header;
//    } else {
//      // We will no longer exist :(. Remove us from the index.
//      u32int iterator = 0;
//      while ((iterator < heap->index.size) &&
//          (lookup_ordered_array(iterator, &heap->index) != (void *) test_header))
//        iterator++;
//      // If we didn't find ourselves, we have nothing to remove.
//      if (iterator < heap->index.size)
//        remove_ordered_array(iterator, &heap->index);
//    }
//  }

  // If required, add us to the index.
  if (do_add == 1)
    insert_ordered_array((void *) header, &heap->index);

}