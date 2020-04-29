class RemoveSoilMoistureFromMeasures < ActiveRecord::Migration[6.0]
  def change

    remove_column :measures, :soil_moisture, :integer
  end
end
