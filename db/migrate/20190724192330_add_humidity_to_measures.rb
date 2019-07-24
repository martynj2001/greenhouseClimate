class AddHumidityToMeasures < ActiveRecord::Migration[5.2]
  def change
    add_column :measures, :humidity_out, :decimal
    add_column :measures, :humidity_in, :decimal
  end
end
