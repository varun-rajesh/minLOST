import re
import matplotlib.pyplot as plt

def is_almost_integer(value, tolerance=0.05):
    """Check if a value is within the tolerance of an integer."""
    return abs(value - round(value)) <= tolerance

def process_file(file_path):
    """Process the file, count lines with almost integer values, and record errors."""
    almost_integer_count = 0
    total_lines = 0
    ra_errors = []
    de_errors = []
    roll_errors = []
    max_error = 0  # To store the maximum error

    # Regular expression to match lines with Ra, De, and Roll values
    pattern = re.compile(r"Ra:\s*([-+]?\d+\.\d+),\s*De:\s*([-+]?\d+\.\d+),\s*Roll:\s*([-+]?\d+\.\d+)")

    with open(file_path, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                total_lines += 1
                ra, de, roll = map(float, match.groups())
                # Calculate errors
                ra_error = abs(ra - round(ra))
                de_error = abs(de - round(de))
                roll_error = abs(roll - round(roll))

                ra_errors.append(ra_error)
                de_errors.append(de_error)
                roll_errors.append(roll_error)

                # Update max error
                max_error = max(max_error, ra_error, de_error, roll_error)

                # Check if all three values are almost integers
                if is_almost_integer(ra) and is_almost_integer(de) and is_almost_integer(roll):
                    almost_integer_count += 1

    print(f"Total lines with Ra, De, Roll values: {total_lines}")
    print(f"Lines with almost integer values: {almost_integer_count}")
    print(f"Maximum error seen: {max_error:.6f}")

    # Plot histograms for Ra, De, and Roll errors
    fig, axes = plt.subplots(3, 1, figsize=(10, 18))  # Increased vertical figure size for better spacing
    plt.subplots_adjust(hspace=0.4)  # Add more space between subplots

    # Ra Errors Histogram
    axes[0].hist(ra_errors, bins=20, color='b', alpha=0.7, label='Ra Errors')
    axes[0].axvline(0.05, color='r', linestyle='--', label='Tolerance (±0.05)')
    axes[0].set_ylabel("Frequency")
    axes[0].legend()
    axes[0].grid()

    # De Errors Histogram
    axes[1].hist(de_errors, bins=20, color='g', alpha=0.7, label='De Errors')
    axes[1].axvline(0.05, color='r', linestyle='--', label='Tolerance (±0.05)')
    axes[1].set_ylabel("Frequency")
    axes[1].legend()
    axes[1].grid()

    # Roll Errors Histogram
    axes[2].hist(roll_errors, bins=20, color='purple', alpha=0.7, label='Roll Errors')
    axes[2].axvline(0.05, color='r', linestyle='--', label='Tolerance (±0.05)')
    axes[2].set_ylabel("Frequency")
    axes[2].legend()
    axes[2].grid()

    plt.tight_layout()  # Adjust the layout for better fit
    plt.show()

if __name__ == "__main__":
    # Replace 'input.txt' with your file's name
    file_path = "star_1000.log"
    process_file(file_path)
