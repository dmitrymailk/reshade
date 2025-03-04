import sys
import os

path = "F:\\project\\programming\\reshade\examples\\01-framerate_limit_copy\\reshade_diffusion\\auto_remaster"
# sys.path.append(
#     "F:\\project\\programming\\reshade\examples\\01-framerate_limit_copy\\reshade_diffusion\\auto_remaster"
# )
sys.path.append(path)
os.chdir(path)

import win32event
import win32con
import time
import os
import win32api
import multiprocessing.shared_memory
import numpy as np

import numpy as np


from PIL import Image
import torch


from torchvision import transforms


import torchvision.transforms.functional as F


from img2img_turbo.src.pix2pix_turbo import Pix2PixLight


from img2img_turbo.src.image_prep import canny_from_pil


from img2img_turbo.src.benchmark import merge_loras

use_fp16 = not False


# initialize the model


model = Pix2PixLight(dtype=torch.float16)


model.set_eval()


PythonSemaphore = "PythonSemaphore"  # Must be consistent
CppSemaphore = "CppSemaphore"
# Create the named semaphore
python_sem = win32event.CreateSemaphore(None, 0, 2, PythonSemaphore)
# None : Security Attributes, 0: Initial Count , 2: Maximum count , "Semaphor Name"
SHARED_MEMORY_NAME = "shared_image"

# ARRAY_SIZE = 1920*1080*4
ARRAY_SIZE = 2560 * 1440 * 4
# ARRAY_SIZE = 1920
HEIGHT = 1440
WIDTH = 2560
SHARED_MEMORY_SIZE = ARRAY_SIZE * 1

if python_sem is None:
    print(f"Error creating semaphore: {win32event.GetLastError()}")
    exit(1)

print("Semaphore created in Python. Waiting for signals...")

global_counter = 0

import numpy as np


from PIL import Image
import torch


from torchvision import transforms


import torchvision.transforms.functional as F
import os

print(os.getcwd())

model.set_eval()
# nfs_pix2pix_1739912522 - более стабильна
model.unet = model.unet.from_pretrained(
    # "img2img_turbo/output/pix2pix_light/nfs_pix2pix_1740323104/model_15001/unet"
    "img2img_turbo/output/pix2pix_light/nfs_pix2pix_1739912522/model_15001/unet"
)
model.vae = model.vae.from_pretrained(
    # "img2img_turbo/output/pix2pix_light/nfs_pix2pix_1740323104/model_15001/vae"
    "img2img_turbo/output/pix2pix_light/nfs_pix2pix_1739912522/model_15001/vae"
)
model.unet = model.unet.to(model.sched.betas.dtype).cuda()
model.vae = model.vae.to(model.sched.betas.dtype).cuda()
model.unet.requires_grad_(False)
model.vae.requires_grad_(False)

aspect_ratio = WIDTH / HEIGHT
new_width = int(512 * aspect_ratio)
# cuda_image = torch.tensor(np.array(input_image)).permute(2, 0, 1).cuda()
cuda_image_t = transforms.Compose(
    [
        transforms.Resize((512, new_width), antialias=not False),
        transforms.CenterCrop(512),
    ]
)

original_height = HEIGHT
original_width = WIDTH
input_height = 512
input_width = 512

pad_top = (original_height - input_height) // 2
pad_left = (original_width - input_width) // 2

black_image = torch.zeros(
    (original_height, original_width, 3),
    dtype=torch.uint8,
    device="cuda",
)

initial_image = torch.ones((HEIGHT, WIDTH, 4), dtype=torch.uint8)
initial_image = 255 * initial_image

while True:
    # Wait for the semaphore to be signaled
    result = win32event.WaitForSingleObject(python_sem, win32event.INFINITE)
    if result == win32con.WAIT_OBJECT_0:
        cpp_sem = win32event.OpenSemaphore(
            win32event.SYNCHRONIZE | win32event.EVENT_MODIFY_STATE,
            False,
            CppSemaphore,
        )
        # DO SOME WORK
        shm = multiprocessing.shared_memory.SharedMemory(
            name=SHARED_MEMORY_NAME, create=False, size=SHARED_MEMORY_SIZE
        )
        # --------------
        # SIMPLE NUMPY TEST
        # int_array = np.ndarray((ARRAY_SIZE,), dtype=np.uint8, buffer=shm.buf)
        # int_array[:] = 255 - int_array
        # --------------
        # --------------
        # SIMPLE PYTORCH TEST
        # cpu_image = torch.frombuffer(
        #     shm.buf,
        #     dtype=torch.uint8,
        # )
        # cuda_image = cpu_image.cuda()
        # cuda_image = 255 - cuda_image
        # cpu_image[:] = cuda_image.cpu()
        # cpu_image.copy_(cuda_image.cpu())
        # --------------
        cpu_image = torch.frombuffer(
            shm.buf,
            dtype=torch.uint8,
        )
        cuda_image = cpu_image.cuda().reshape((HEIGHT, WIDTH, 4))
        cuda_image = cuda_image[:, :, [2, 1, 0]]
        original_cuda_image = cuda_image.clone()

        cuda_image = cuda_image_t(cuda_image.permute(2, 0, 1)).unsqueeze(0)
        original_cuda_image = cuda_image_t(
            original_cuda_image.permute(2, 0, 1)
        ).unsqueeze(0)
        cuda_image = (cuda_image / 255).to(torch.float16)
        cuda_image = model(cuda_image)
        # cuda_image = (cuda_image * 0.5 + 0.5) * 255
        cuda_image.mul_(0.5).add_(0.5).mul_(255)
        cuda_image.squeeze_(0)
        cuda_image = (cuda_image).permute(1, 2, 0).to(torch.uint8)
        cuda_image = cuda_image[:, :, [2, 1, 0]]

        original_cuda_image.squeeze_(0)
        original_cuda_image = original_cuda_image.permute(1, 2, 0)
        original_cuda_image = original_cuda_image[:, :, [2, 1, 0]]

        # black_image[
        #     pad_top : pad_top + input_height, pad_left : pad_left + input_width, :
        # ] = cuda_image
        original_image_x_offset = pad_left // 2
        transformed_image_x_offset = pad_left // 2 + input_width  # Shift to the right

        # Check if enough space is available
        # if transformed_image_x_offset + input_width > black_image.shape[1]:
        #     print("Warning: Not enough space in black_image for both images side-by-side. Consider increasing width of black_image")

        black_image[
            pad_top : pad_top + input_height,
            original_image_x_offset : original_image_x_offset + input_width,
            :,
        ] = original_cuda_image

        black_image[
            pad_top : pad_top + input_height,
            transformed_image_x_offset : transformed_image_x_offset + input_width,
            :,
        ] = cuda_image

        initial_image[:, :, :3] = black_image
        cpu_image.copy_(initial_image.flatten().cpu())
        global_counter += 1
        print("Python work ", global_counter)
        win32event.ReleaseSemaphore(cpp_sem, 1)

        print("Work done. Waiting again...")  # Signal ready
    elif result == win32con.WAIT_ABANDONED:
        print("Semaphore abandoned! Exiting...")
        break  # Exit loop if abandoned
    else:
        print(f"Wait failed: {result}")
        break  # Exit loop
