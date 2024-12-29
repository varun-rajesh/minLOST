import re
import matplotlib.pyplot as plt

def parse_logs(reference_file, generated_file):
    """Parse logs to calculate RA, Dec, Roll errors and generate metrics."""
    reference_data = {}
    errors = {"RA": [], "Dec": [], "Roll": []}
    valid_iterations = 0
    ra_dec_tolerance = 0.05
    roll_tolerance = 0.1
    large_tolerance_count = 0
    insufficient_stars = 0
    insufficient_identifications = 0

    # Parse reference log
    with open(reference_file, 'r') as file:
        for line in file:
            match = re.match(r"Iteration=(\d+), RA=([\d\.\-]+), Dec=([\d\.\-]+), Roll=([\d\.\-]+)", line)
            if match:
                iteration = int(match.group(1))
                ra = float(match.group(2))
                dec = float(match.group(3))
                roll = float(match.group(4))
                reference_data[iteration] = {"RA": ra, "Dec": dec, "Roll": roll}

    # Parse generated log and compute metrics
    with open(generated_file, 'r') as file:
        for line in file:
            insufficient_stars_match = re.search(r"Insufficient stars", line)
            insufficient_identifications_match = re.search(r"Couldn't identify 4 stars", line)

            if insufficient_stars_match:
                insufficient_stars += 1
            elif insufficient_identifications_match:
                insufficient_identifications += 1

            match = re.match(r"Iteration=(\d+), RA=([\d\.\-]+), Dec=([\d\.\-]+), Roll=([\d\.\-]+)", line)
            if match:
                iteration = int(match.group(1))
                ra = float(match.group(2))
                dec = float(match.group(3))
                roll = float(match.group(4))

                if iteration in reference_data:
                    ref_values = reference_data[iteration]
                    ra_error = abs(ref_values["RA"] - ra)
                    dec_error = abs(ref_values["Dec"] - dec)
                    roll_error = abs(ref_values["Roll"] - roll)

                    errors["RA"].append(ra_error)
                    errors["Dec"].append(dec_error)
                    errors["Roll"].append(roll_error)

                    # Check for valid iterations
                    if ra_error < ra_dec_tolerance and dec_error < ra_dec_tolerance and roll_error < roll_tolerance:
                        valid_iterations += 1
                    else:
                        large_tolerance_count += 1

    return errors, valid_iterations, large_tolerance_count, insufficient_stars, insufficient_identifications

def plot_histograms(errors):
    """Plot histograms of RA, Dec, Roll errors."""
    fig, axes = plt.subplots(3, 1, figsize=(10, 18))
    plt.subplots_adjust(hspace=0.4)

    # Plot RA errors
    axes[0].hist(errors["RA"], bins=20, color='b', alpha=0.7, label='RA Errors')
    axes[0].axvline(0.05, color='r', linestyle='--', label='RA Tolerance (±0.05)')
    axes[0].set_title("Histogram of RA Errors")
    axes[0].set_xlabel("Error (degrees)")
    axes[0].set_ylabel("Frequency")
    axes[0].legend()
    axes[0].grid()

    # Plot Dec errors
    axes[1].hist(errors["Dec"], bins=20, color='g', alpha=0.7, label='Dec Errors')
    axes[1].axvline(0.05, color='r', linestyle='--', label='Dec Tolerance (±0.05)')
    axes[1].set_title("Histogram of Dec Errors")
    axes[1].set_xlabel("Error (degrees)")
    axes[1].set_ylabel("Frequency")
    axes[1].legend()
    axes[1].grid()

    # Plot Roll errors
    axes[2].hist(errors["Roll"], bins=20, color='purple', alpha=0.7, label='Roll Errors')
    axes[2].axvline(0.1, color='r', linestyle='--', label='Roll Tolerance (±0.1)')
    axes[2].set_title("Histogram of Roll Errors")
    axes[2].set_xlabel("Error (degrees)")
    axes[2].set_ylabel("Frequency")
    axes[2].legend()
    axes[2].grid()

    plt.tight_layout()
    plt.show()

def main():
    reference_file = "image_gen_float.log"
    generated_file = "star_float_1000.log"

    # Parse logs and calculate errors
    errors, valid_iterations, large_tolerance_count, insufficient_stars, insufficient_identifications = parse_logs(
        reference_file, generated_file
    )

    # Print metrics
    print(f"Number of valid iterations (RA/Dec error < 0.05, Roll error < 0.1 degrees): {valid_iterations}")
    print(f"Number of iterations with errors exceeding tolerances: {large_tolerance_count}")
    print(f"Number of iterations with insufficient stars: {insufficient_stars}")
    print(f"Number of iterations with sufficient stars but couldn't identify 4 stars: {insufficient_identifications}")

    # Plot histograms
    plot_histograms(errors)

if __name__ == "__main__":
    main()
