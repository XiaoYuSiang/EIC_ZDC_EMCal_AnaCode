import sys
from PIL import Image

def stack_images(image_files, output_file):
    if len(image_files) != 3:
        print("Error: Exactly 3 image files are required.")
        sys.exit(1)
    
    # Define image sizes
    pixX, pixY = 900, 400  # Small images (900x400)
    pixX_large, pixY_large = 900, 800  # Large image (900x800)
    
    try:
        # Load images
        img3 = Image.open(image_files[0]).resize((pixX_large, pixY_large))
        img1 = Image.open(image_files[1]).resize((pixX, pixY))
        img2 = Image.open(image_files[2]).resize((pixX, pixY))
    except FileNotFoundError as e:
        print(f"Error: {e}")
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}")
        sys.exit(1)
    
    # Create a blank canvas with the total required size
    final_image = Image.new("RGB", (pixX + pixX_large, pixY * 2), (255, 255, 255))
    
    # Paste images onto the blank canvas
    final_image.paste(img1, (0, 0))       # Top-left (small)
    final_image.paste(img2, (0, pixY))    # Bottom-left (small)
    final_image.paste(img3, (pixX, 0))    # Right (large)
    
    # Save final image
    try:
        final_image.save(output_file)
        print(f"Stacked image saved to {output_file}")
    except Exception as e:
        print(f"Error saving image: {e}")
        sys.exit(1)

if __name__ == "__main__":
    args = sys.argv[1:]
    if len(args) < 4:
        print("Usage: python stack_images.py <img1> <img2> <img3> <output_file>")
        sys.exit(1)
    
    image_files = args[:-1]  # First 3 arguments are image paths
    output_file = args[-1]   # Last argument is output file path
    
    stack_images(image_files, output_file)