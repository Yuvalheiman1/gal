
import pandas as pd

# Replace 'file_path' with the actual path to your file
file_path = "C:\\Users\\yuval\\shcool\\UI\\test\\test_project\\Y-2024.csv"

# Load the CSV file
data = pd.read_csv(file_path)

# Get unique values in the "determinand.definition" column
unique_definitions = data['determinand.definition'].unique()

# Print unique values and count
print(f"Number of unique determinand definitions: {len(unique_definitions)}")
print("Unique determinand definitions:")
print(unique_definitions)