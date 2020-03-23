class AddSoilMoistureToMeasures < ActiveRecord::Migration[5.2]
  def change
    add_column :measures, :soil_moisture, :integer
  end
end
