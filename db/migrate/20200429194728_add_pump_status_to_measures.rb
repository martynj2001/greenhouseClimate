class AddPumpStatusToMeasures < ActiveRecord::Migration[6.0]
  def change
    add_column :measures, :pump_status, :string
  end
end
