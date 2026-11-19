# Memory management in linux
## Kernel memory allocation APIs
include/linux/slab.h
### kmalloc
``` C
void *kmalloc(size_t size, gfp_t flags);
```
### kmalloc_array
``` C
void *kmalloc_array(size_t n, size_t size, gfp_t flags);
``` 
### kcalloc
``` C
void *kcalloc(size_t n, size_t size, gfp_t flags);
``` 
### kzalloc
``` C
void *kzalloc(size_t size, gfp_t flags);
``` 
### krealloc
``` C
void *krealloc(const void *p, size_t new_size, gfp_t flags);
``` 
### kfree
``` C
void kfree(const void *objp);
``` 
### gfp_t
| gfp_t | Ý nghĩa chính |
|---|---|
| `GFP_KERNEL` | Có thể sleep để chờ thu hồi bộ nhớ; ngữ cảnh gọi phải cho phép |
| `GFP_NOWAIT` | Không chờ direct reclaim; cần xử lý khả năng thất bại |
| `GFP_ATOMIC` | Không sleep, có thể dùng một phần dự trữ; vẫn có thể thất bại |
<!-- # Resource managed kernel APIs -->
<!-- devm_kmalloc() -->